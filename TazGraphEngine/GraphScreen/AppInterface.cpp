#include "AppInterface.h"
#include "../BaseFPSLimiter/BaseFPSLimiter.h"

#include "SceneList.h"
#include "IScene.h"

#include "../DataManager/DataManager.h"
#include "../Camera2.5D/CameraManager.h"
#include <chrono>

using namespace std::chrono;

AppInterface::AppInterface(int threadCount):threadPool(threadCount) {
	_sceneList = std::make_unique<SceneList>(this);
}

AppInterface::~AppInterface() {

}

void AppInterface::run() {

	const float DESIRED_FPS = 60;
	const int MAX_PHYSICS_STEPS = 6;

	if (!init()) return;

	const float MS_PER_SECOND = 1000;
	const float DESIRED_FRAMETIME = MS_PER_SECOND / DESIRED_FPS;
	const float MAX_DELTA_TIME = 1.0f;

	Uint64 freq = SDL_GetPerformanceFrequency();
	Uint64 prevTicks = SDL_GetPerformanceCounter();

	_limiter.setMaxFPS(100.0f);

	_isRunning = true;
	while (_isRunning) {
		_limiter.begin();

		Uint64 newTicks = SDL_GetPerformanceCounter();
		float frameTime = static_cast<float>(newTicks - prevTicks) / freq * 1000.0f; // in ms
		prevTicks = newTicks;
		float totalDeltaTime = frameTime / DESIRED_FRAMETIME;

		int i = 0;
		while (totalDeltaTime > 0.0f && i < MAX_PHYSICS_STEPS) {

			Uint64 startInput = SDL_GetPerformanceCounter();
			checkInput();
			Uint64 endInput = SDL_GetPerformanceCounter();
			float inputTime = static_cast<float>(endInput - startInput) / freq * 1000.0f;

			Uint64 startUpdate = SDL_GetPerformanceCounter();
			float deltaTime = std::min(totalDeltaTime, MAX_DELTA_TIME);
			update(deltaTime);
			Uint64 endUpdate = SDL_GetPerformanceCounter();
			float updateTime = static_cast<float>(endUpdate - startUpdate) / freq * 1000.0f;

			totalDeltaTime -= deltaTime;
			i++;

			if (_isRunning) {
				Uint64 startDraw = SDL_GetPerformanceCounter();
				draw();
				Uint64 endDraw = SDL_GetPerformanceCounter();
				float drawTime = static_cast<float>(endDraw - startDraw) / freq * 1000.0f;

				std::cout << "Input: " << inputTime << " ms, Update: " << updateTime << " ms, Draw: " << drawTime << " ms\n";
			}
		}

		Uint64 startUI = SDL_GetPerformanceCounter();
		updateUI();
		Uint64 endUI = SDL_GetPerformanceCounter();
		float uiTime = static_cast<float>(endUI - startUI) / freq * 1000.0f;

		_window.swapBuffer();

		_limiter.fps = _limiter.end();

		static int frameCounter = 0;
		frameCounter++;
		if (frameCounter == 10) {
			_limiter.setHistoryValue(_limiter.fps);
			frameCounter = 0;
		}

		std::cout << "UI: " << uiTime << " ms, Total Frame Time: " << frameTime << " ms, FPS: " << _limiter.fps << "\n";
	}
}
void AppInterface::exitSimulator() {
	_currentScene->onExit();
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
			float newWidth = evnt.window.data1;
			float newHeight = evnt.window.data2;
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

	_currentScene = _sceneList->getCurrent();
	_currentScene->onEntry();
	_currentScene->setRunning();

	return true;
}

bool AppInterface::initSystems() {
	_window.create("Taz Graph", 800.0f, 640.0f, 1.0f, TazGraphEngine::VISIBLE);
	return true;
}

void AppInterface::checkInput() {
	if (_currentScene) {
		_inputManager.update();

		switch (_currentScene->getState()) {
		case SceneState::RUNNING:
			_currentScene->checkInput();
			break;
		default:
			break;
		}
	}
}

void AppInterface::update(float deltaTime) {
	if (_currentScene) {
		switch (_currentScene->getState()) {
		case SceneState::RUNNING:
			_currentScene->update(deltaTime);
			break;
		case SceneState::CHANGE_NEXT:
			_currentScene->onExit();
			_currentScene = _sceneList->moveNext();
			if (_currentScene) {
				_currentScene->setRunning();
				_currentScene->onEntry();
			}
			break;
		case SceneState::CHANGE_PREVIOUS:
			_currentScene->onExit();
			_currentScene = _sceneList->movePrevious();
			if (_currentScene) {
				_currentScene->setRunning();
				//_currentScene->onEntry();
			}
			break;
		case SceneState::EXIT_APPLICATION:
			exitSimulator();
			break;
		default:
			break;
		}
	}
	else {
		exitSimulator();
	}

}
void AppInterface::draw() {
	glViewport(0, 0, _window.getScreenWidth(), _window.getScreenHeight());
	if (_currentScene && _currentScene->getState() == SceneState::RUNNING) {
		_currentScene->draw();
	}
}

void AppInterface::updateUI()
{
	// Start the Dear ImGui frame
	_currentScene->BeginRender();
	if (_currentScene && _currentScene->getState() == SceneState::RUNNING) {
		_currentScene->updateUI();
	}
	// Rendering
	_currentScene->EndRender();
}
