#include "AppInterface.h"
#include "../BaseFPSLimiter/BaseFPSLimiter.h"

#include "SceneList.h"
#include "IScene.h"

#include "../DataManager/DataManager.h"



using namespace std::chrono;

AppInterface::AppInterface(int threadCount, std::string m_openFile,
	double m_initialTimestamp,
	int m_initialStep) :
	threadPool(threadCount),
	openFile(m_openFile),
	initialTimestamp(m_initialTimestamp),
	initialStep(m_initialStep) {
	_sceneList = std::make_unique<SceneList>(this);
}

AppInterface::~AppInterface() {

}

void AppInterface::run() {

	const float DESIRED_FPS = 60;
	const int MAX_PHYSICS_STEPS = 1;
	//SDL_GL_MakeCurrent(_window._sdlWindow, _window.glContext);

	if (!init()) return;

	const float MS_PER_SECOND = 1000;
	const float DESIRED_FRAMETIME = MS_PER_SECOND / DESIRED_FPS;
	const float MAX_DELTA_TIME = 1.0f;

	int frameCounter = 0;

	renderThread = std::thread(&AppInterface::RenderThreadFunc, this);

	Uint64 freq = SDL_GetPerformanceFrequency();
	Uint64 prevTicks = SDL_GetPerformanceCounter();

	_limiter.setMaxFPS(60.0f);
	//SDL_GL_MakeCurrent(_window._sdlWindow, _window.glContext);

	_isRunning = true;
	while (_isRunning) {
		ZoneScoped;
		FrameMark;

		_limiter.begin();

		Uint64 newTicks = SDL_GetPerformanceCounter();
		float frameTime = static_cast<float>(newTicks - prevTicks) / freq * 1000.0f; // in ms
		prevTicks = newTicks;
		float totalDeltaTime = frameTime / DESIRED_FRAMETIME;


		{
			ZoneScopedN("Input"); // Profile input section
			Uint64 startInput = SDL_GetPerformanceCounter();
			checkInput();
			Uint64 endInput = SDL_GetPerformanceCounter();
			float inputTime = static_cast<float>(endInput - startInput) / freq * 1000.0f;
			//std::cout << "Input: " << inputTime << " ms" << std::endl;
		}
		int i = 0;

		while (totalDeltaTime > 0.0f && i < MAX_PHYSICS_STEPS) {
			ZoneScopedN("Physics Step"); // Profile physics loop

			Uint64 startUpdate = SDL_GetPerformanceCounter();
			float deltaTime = std::min(totalDeltaTime, MAX_DELTA_TIME);
			{
				ZoneScopedN("Update");
				update(deltaTime);
			}

			{
				ZoneScopedN("UpdateUI");
				updateUI(deltaTime);
			}
			Uint64 endUpdate = SDL_GetPerformanceCounter();
			float updateTime = static_cast<float>(endUpdate - startUpdate) / freq * 1000.0f;

			totalDeltaTime -= deltaTime;
			i++;
			//std::cout << "Update: " << updateTime << " ms\n";


		}




		if (_isRunning) {
			ZoneScopedN("PrepareDraw");
			Uint64 startDraw = SDL_GetPerformanceCounter();
			prepareDraw();
			Uint64 endDraw = SDL_GetPerformanceCounter();
			float drawTime = static_cast<float>(endDraw - startDraw) / freq * 1000.0f;
			//std::cout << "Draw: " << drawTime << " ms\n";
		}
		if (_isRunning) {
			ZoneScopedN("RenderDraw");
			// Get the write index (opposite of active)
			int writeIndex = 1 - activeIndex.load();

			queues[writeIndex].Submit([this]() {
				ZoneScopedN("Draw");
				renderDraw();
				});
			queues[writeIndex].Submit([this]() {
				ZoneScopedN("DrawUI");
				drawUI();  // This calls ImGui rendering - MUST be on render thread
				});
			// Swap buffers - make write buffer active
			activeIndex.store(writeIndex);

			// Signal render thread that frame is ready
			frameReady.store(true);
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
		//std::cout << "UI: " << uiTime << " ms, Total Frame Time: " << frameTime << " ms, FPS: " << _limiter.fps << "\n";
	}
	//SDL_GL_MakeCurrent(_window._sdlWindow, nullptr);
}

void AppInterface::RenderThreadFunc() {
	// Make OpenGL context current on this thread
	SDL_GL_MakeCurrent(_window._sdlWindow, _window.glContext);

	resourceManager.addGLSLProgram("texture");
	resourceManager.addGLSLProgram("color");

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		//InitShaders function from Bengine
		resourceManager.getGLSLProgram("texture")->compileAndLinkShaders("Src/Shaders/textureBright.vert", "Src/Shaders/textureBright.frag");
		resourceManager.getGLSLProgram("texture")->addAttribute("vertexPosition");
		resourceManager.getGLSLProgram("texture")->addAttribute("vertexColor");
		resourceManager.getGLSLProgram("texture")->addAttribute("vertexUV");

		resourceManager.getGLSLProgram("color")->compileAndLinkShaders("Src/Shaders/colorShading.vert", "Src/Shaders/colorShading.frag");
		resourceManager.getGLSLProgram("color")->addAttribute("vertexPosition");
		resourceManager.getGLSLProgram("color")->addAttribute("vertexColor");
		resourceManager.getGLSLProgram("color")->addAttribute("vertexUV");
	}

	while (_isRunning) {
		while (!frameReady.load() && _isRunning) {
			std::this_thread::sleep_for(std::chrono::microseconds(100));
		}

		if (!_isRunning) break;

		// Get the active queue to read from
		int readIndex = activeIndex.load();

		// Execute all rendering commands
		{
			ZoneScopedN("Execute Render Commands");
			queues[readIndex].Execute();
		}

		// Swap OpenGL buffers - ONLY on render thread
		{
			ZoneScopedN("SwapBuffer");
			_window.swapBuffer();
		}

		// Mark frame as consumed
		frameReady.store(false);
	}

	// Cleanup ImGui before exiting
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();

	SDL_GL_MakeCurrent(_window._sdlWindow, nullptr);
}

void AppInterface::exitSimulator() {
	if (!_sceneList || !_sceneList->getCurrent())
		return;

	_sceneList->getCurrent()->onExit();
	if (_sceneList) {
		_sceneList->destroy();
		_sceneList.reset();
	}
	_isRunning = false;

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
		//std::cout << event.motion.x << " " << event.motion.y << std::endl;
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
	SDL_GL_MakeCurrent(_window._sdlWindow, _window.glContext);

	initRenderers();

	_sceneList->getCurrent()->onEntry();
	_sceneList->getCurrent()->setRunning();
	SDL_GL_MakeCurrent(_window._sdlWindow, nullptr);

	return true;
}

void AppInterface::initRenderers() {
	planeModelRenderer.init();
	generateSphereMesh(
		lightRenderer.sphereVertices,
		lightRenderer.sphereIndices);

	lightRenderer.init();

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

	switch (_sceneList->getCurrent()->getState()) {
	case SceneState::RUNNING:
		_sceneList->getCurrent()->update(deltaTime);
		break;
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
	case SceneState::EXIT_APPLICATION:
		exitSimulator();
		break;
	default:
		break;
	}

}

void AppInterface::prepareDraw()
{
	glViewport(0, 0, _window.getScreenWidth(), _window.getScreenHeight());
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
