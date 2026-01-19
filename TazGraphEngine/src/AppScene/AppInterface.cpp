#include "AppInterface.h"
#include "../BaseFPSLimiter/BaseFPSLimiter.h"

#include "SceneList.h"
#include "IScene.h"

#include "../DataManager/DataManager.h"


using namespace std::chrono;

AppInterface::AppInterface(int threadCount,
	int msaa_samples,
	std::string m_openFile,
	double m_initialTimestamp,
	int m_initialStep,
	bool m_usePython,
	std::array<float, 4> m_bg,
	bool m_useGrid) :
	threadPool(threadCount),
	MSAA_samples(msaa_samples),
	openFile(m_openFile),
	initialTimestamp(m_initialTimestamp),
	initialStep(m_initialStep),
	usePython(m_usePython),
	backgroundColor(m_bg),
	useGrid(m_useGrid)
{

	if (msaa_samples > 1) {
		useMSAA = true;
	}

	_sceneList = std::make_unique<SceneList>(this);
}

AppInterface::~AppInterface() {

}

void AppInterface::run() {

	const float DESIRED_FPS = 60;
	const int MAX_PHYSICS_STEPS = 1;

	if (!init()) return;


	const float MS_PER_SECOND = 1000;
	const float DESIRED_FRAMETIME = MS_PER_SECOND / DESIRED_FPS;
	const float MAX_DELTA_TIME = 1.0f;

	int frameCounter = 0;


	Uint64 freq = SDL_GetPerformanceFrequency();
	Uint64 prevTicks = SDL_GetPerformanceCounter();

	_limiter.setMaxFPS(60.0f);

	while (_isRunning) {
		ZoneScoped;
		FrameMark;

		_limiter.begin();

		Uint64 newTicks = SDL_GetPerformanceCounter();
		float frameTime = static_cast<float>(newTicks - prevTicks) / freq * 1000.0f; // in ms
		prevTicks = newTicks;
		float totalDeltaTime = frameTime / DESIRED_FRAMETIME;

		{
			checkInput();
		};

		int i = 0;

		while (totalDeltaTime > 0.0f && i < MAX_PHYSICS_STEPS) {
			float deltaTime = std::min(totalDeltaTime, MAX_DELTA_TIME);
			{
				update(deltaTime);
			}

			{
				updateUI(deltaTime);
			}

			totalDeltaTime -= deltaTime;
			i++;


		}

		if (_isRunning) {
			{
				std::unique_lock<std::mutex> lock(frameMutex);
				frameConsumedCV.wait(lock, [this]() {
					return frameConsumed.load() || !_isRunning;
					});

				if (!_isRunning) break;
			}

			prepareDraw();

			int writeIndex = 1 - activeIndex.load();
			activeIndex.store(writeIndex);

		}
		if (_isRunning) {
			// Get the write index (opposite of active)
			int readIndex = activeIndex.load();

			queues[readIndex].Submit([this]() {
				// Process ImGui events on render thread
				{
					std::lock_guard<std::mutex> lock(imguiEventsMutex);
					for (auto& event : imguiEvents) {
						ImGui_ImplSDL2_ProcessEvent(&event);
					}
				}
				renderDraw();
				});
			queues[readIndex].Submit([this]() {
				drawUI();  // This calls ImGui rendering - MUST be on render thread
				});
			queues[readIndex].Submit([this]() {
				swapBuffer();  // This calls ImGui rendering - MUST be on render thread
				});

			// Signal frame ready
			{
				std::lock_guard<std::mutex> lock(frameMutex);
				frameConsumed.store(false);
				frameReady.store(true);
			}
			frameReadyCV.notify_one();
		}

		_limiter.end();

		frameCounter++;
		if (frameCounter == 10) {
			_limiter.setHistoryValue(_limiter.fps);
			frameCounter = 0;
		}
		TracyPlot("FPS", _limiter.fps); // Plot FPS over time
		TracyPlot("Frame Time (ms)", frameTime); // Plot frame time
		FrameMark;
	}
}

void AppInterface::enqueueRenderCommand(std::function<void()> cmd) {
	{
		std::lock_guard<std::mutex> lock(initMutex);
		initQueue.Submit(std::move(cmd));
		initCommandReady.store(true);
	}
	initCV.notify_one();
}

void AppInterface::waitForRenderCommand() {
	TAZ_LOG("Waiting for render command...");
	std::unique_lock<std::mutex> lock(initMutex);
	initCV.wait(lock, [this]() {
		return initCommandComplete.load();
		});
	initCommandComplete.store(false);
}
void AppInterface::waitForRenderThreadExit() {
	if (renderThread.joinable()) {

		int writeIndex = 1 - activeIndex.load();

		queues[writeIndex].Submit([this]() {
			planeModelRenderer.dispose();
			lineRenderer.dispose();
			planeColorRenderer.dispose();
			lightRenderer.dispose();
			resourceManager.disposeGLSLPrograms();
			});

		// Signal the render thread to exit
		_isRunning = false;

		// Wake it up if it's waiting on any condition variable
		{
			std::lock_guard<std::mutex> lock(frameMutex);
			frameReadyCV.notify_all();
			frameConsumedCV.notify_all();
		}
		{
			std::lock_guard<std::mutex> lock(initMutex);
			initCV.notify_all();
		}

		// Wait for render thread to finish
		renderThread.join();
		TAZ_LOG("[Main] Render thread has exited cleanly.\n");
	}
}

void AppInterface::RenderThreadFunc() {
	SDL_GL_MakeCurrent(_window._sdlWindow, _window.glContext);
	tracy::SetThreadName("Render Thread");

	while (_isRunning)
	{
		bool shouldProcessFrame = false;
		bool shouldProcessInit = false;

		{
			std::unique_lock<std::mutex> lock(frameMutex);

			// Wait for either frame ready OR a short timeout to check init commands
			frameReadyCV.wait_for(lock, std::chrono::milliseconds(1), [this]() {
				return frameReady.load() || !_isRunning;
				});

			if (!_isRunning) break;

			if (frameReady.load()) {
				shouldProcessFrame = true;
			}
		}

		// Check for init commands (without blocking frame rendering)
		{
			std::lock_guard<std::mutex> lock(initMutex);
			if (initCommandReady.load()) {
				shouldProcessInit = true;
			}
		}

		// Process init commands first (higher priority)
		if (shouldProcessInit) {
			std::lock_guard<std::mutex> lock(initMutex);
			if (initCommandReady.load()) {
				TAZ_LOG("Executing init command");
				initQueue.Execute();
				initCommandReady.store(false);
				initCommandComplete.store(true);
				initCV.notify_one();
			}
		}

		// Process frame rendering
		if (shouldProcessFrame) {
			int readIndex = activeIndex.load();

			{
				ZoneScopedN("Execute Render Commands");
				queues[readIndex].Execute();
			}

			{
				std::lock_guard<std::mutex> lock(frameMutex);
				frameReady.store(false);
				frameConsumed.store(true);
			}
			frameConsumedCV.notify_one();
		}
	}

	// Cleanup ImGui before exiting
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();

	SDL_GL_MakeCurrent(_window._sdlWindow, nullptr);


}

void AppInterface::exitSimulator() {
	waitForRenderThreadExit();
	if (!_sceneList || !_sceneList->getCurrent())
		return;

	_sceneList->getCurrent()->onExit();
	if (_sceneList) {
		_sceneList->destroy();
		_sceneList.reset();
	}

	// Set running flag and NOTIFY the condition variable
	for (Thread& thread : threadPool.threads) {
		thread.stop();
	}
}

void AppInterface::onSDLEvent(SDL_Event& evnt) {
	switch (evnt.type)
	{
	case SDL_QUIT:
		exitSimulator();
		break;
	case SDL_KEYDOWN:
		_inputManager.pressKey(evnt.key.keysym.sym);
		break;
	case SDL_KEYUP:
		_inputManager.releaseKey(evnt.key.keysym.sym);
		break;
	case SDL_MOUSEMOTION:
		_inputManager.setMouseCoords(evnt.motion.x / _window.getScale(), evnt.motion.y / _window.getScale());
		break;
	case SDL_MOUSEBUTTONDOWN:
		_inputManager.pressKey(evnt.button.button);
		break;
	case SDL_MOUSEBUTTONUP:
		_inputManager.releaseKey(evnt.button.button);
		break;
	case SDL_WINDOWEVENT:
		switch (evnt.window.event) {
		case SDL_WINDOWEVENT_SIZE_CHANGED:
		case SDL_WINDOWEVENT_RESIZED:
			//? Dont have this on each resize, it messes up viewport sizes
			int newWidth = evnt.window.data1;
			int newHeight = evnt.window.data2;
			// Handle window resizing, update viewport or other necessary elements
			_window.setScreenWidth(newWidth);
			_window.setScreenHeight(newHeight);
			break;
		}
		break;
	default:
		break;
	}

}

bool AppInterface::init() {
	//Initialize SDL
	SDL_Init(SDL_INIT_EVERYTHING);

	//Tell SDL that we want a double buffered window so we dont get
	//any flickering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	if (!initSystems()) return false;

	onInit();

	addScenes();

	_audioEngine.init();

	CameraManager::getInstance().initializeCameras();

	int newWidth = 0;
	int newHeight = 0;

	SDL_GetWindowSize(_window._sdlWindow, &newWidth, &newHeight);
	// Handle window resizing, update viewport or other necessary elements
	//_window.setScreenWidth(newWidth);
	//_window.setScreenHeight(newHeight);

	SDL_GL_MakeCurrent(_window._sdlWindow, _window.glContext);
	initRenderers();
	SDL_GL_MakeCurrent(_window._sdlWindow, nullptr);

	_isRunning = true;
	renderThread = std::thread(&AppInterface::RenderThreadFunc, this);

	_sceneList->getCurrent()->onEntry();
	_sceneList->getCurrent()->setRunning();

	return true;
}

void AppInterface::initRenderers() {
	planeModelRenderer.init();
	generateSphereMesh(
		lightRenderer.sphereVertices,
		lightRenderer.sphereIndices);

	lightRenderer.init();

	generateSphereMeshWireframe(
		lineRenderer.sphereVertices,
		lineRenderer.sphereIndices
	);

	lineRenderer.init();
	generateSphereMesh(
		planeColorRenderer.sphereVertices,
		planeColorRenderer.sphereIndices);
	planeColorRenderer.init();
}

bool AppInterface::initSystems() {
	_window.create("TazGraph", 800, 640, 1.0f, TazGraphEngine::VISIBLE);
	return true;
}

void AppInterface::checkInput() {
	if (!_sceneList || !_sceneList->getCurrent())
		return;

	_inputManager.update();

	switch (_sceneList->getCurrent()->getState()) {
	case SceneState::RUNNING:
		_sceneList->getCurrent()->checkInput();
		break;
	default:
		break;
	}
}

void AppInterface::update(float deltaTime) {
	if (!_sceneList || !_sceneList->getCurrent())
	{
		exitSimulator();
		return;
	}
	while (
		_sceneList->getCurrent()->getState() != SceneState::RUNNING &&
		_sceneList->getCurrent()->getState() != SceneState::EXIT_APPLICATION
		) {
		switch (_sceneList->getCurrent()->getState()) {
		case SceneState::CHANGE_NEXT:
			_sceneList->getCurrent()->onExit();
			_sceneList->moveNext();
			if (_sceneList->getCurrent()) {
				_sceneList->getCurrent()->setRunning();
				_sceneList->getCurrent()->onEntry();
			}
			break;
		case SceneState::CHANGE_PREVIOUS:
			_sceneList->getCurrent()->onExit();
			_sceneList->movePrevious();
			if (_sceneList->getCurrent()) {
				_sceneList->getCurrent()->setRunning();
				//_sceneList->getCurrent()->onEntry();
			}
			break;
		default:
			break;
		}
	}

	switch (_sceneList->getCurrent()->getState()) {
	case SceneState::RUNNING:
		_sceneList->getCurrent()->update(deltaTime);
		break;
	case SceneState::EXIT_APPLICATION:
		exitSimulator();
		break;
	default:
		break;
	}

}

void AppInterface::prepareDraw()
{
	if (!_sceneList || !_sceneList->getCurrent())
		return;

	if (_sceneList->getCurrent()->getState() == SceneState::RUNNING) {
		_sceneList->getCurrent()->prepareDraw();
	}
}

void AppInterface::renderDraw()
{
	glViewport(0, 0, _window.getScreenWidth(), _window.getScreenHeight());
	if (!_sceneList || !_sceneList->getCurrent())
		return;

	if (_sceneList->getCurrent()->getState() == SceneState::RUNNING) {
		_sceneList->getCurrent()->renderDraw();
	}
}

void AppInterface::updateUI(float deltaTime)
{
	if (!_sceneList || !_sceneList->getCurrent())
		return;

	if (_sceneList->getCurrent()->getState() == SceneState::RUNNING) {
		_sceneList->getCurrent()->updateUI(deltaTime);
	}
}

void AppInterface::drawUI()
{
	// Start the Dear ImGui frame
	if (!_sceneList || !_sceneList->getCurrent())
		return;
	_sceneList->getCurrent()->BeginRender();
	if (_sceneList->getCurrent()->getState() == SceneState::RUNNING) {
		_sceneList->getCurrent()->drawUI();
	}
	// Rendering
	_sceneList->getCurrent()->EndRender();
}

void AppInterface::swapBuffer()
{
	if (!_sceneList || !_sceneList->getCurrent())
		return;

	if (_sceneList->getCurrent()->getState() == SceneState::RUNNING) {
		_sceneList->getCurrent()->SwapBufferDraw();
	}
}
