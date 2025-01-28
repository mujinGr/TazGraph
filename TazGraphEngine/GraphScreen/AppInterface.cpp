#include "AppInterface.h"
#include "../BaseFPSLimiter/BaseFPSLimiter.h"

#include "SceneList.h"
#include "IScene.h"

#include "../DataManager/DataManager.h"
#include "../Camera2.5D/CameraManager.h"

AppInterface::AppInterface() {
	_sceneList = std::make_unique<SceneList>(this);
}

AppInterface::~AppInterface() {

}

void AppInterface::run() {

	const float DESIRED_FPS = 60;
	const int MAX_PHYSICS_STEPS = 6;

	if (!init()) return; // get "Game"

	const float MS_PER_SECOND = 1000;
	const float DESIRED_FRAMETIME = MS_PER_SECOND / DESIRED_FPS;
	const float MAX_DELTA_TIME = 1.0f;

	float prevTicks = SDL_GetTicks();

	_limiter.setMaxFPS(1000.0f);

	_isRunning = true;
	while (_isRunning) {
		_limiter.begin();

		float newTicks = SDL_GetTicks();
		float frameTime = newTicks - prevTicks;
		prevTicks = newTicks;
		float totalDeltaTime = frameTime / DESIRED_FRAMETIME;


		int i = 0;
		while (totalDeltaTime > 0.0f && i < MAX_PHYSICS_STEPS)
		{
			float deltaTime = std::min(totalDeltaTime, MAX_DELTA_TIME);
			update(deltaTime); //handleEvents first thing in update
			totalDeltaTime -= deltaTime;
			i++;
			if (_isRunning) {
				draw();
			}
		}

		_limiter.fps = _limiter.end();

		static int frameCounter = 0;
		frameCounter++;
		if (frameCounter == 10) {
			_limiter.setHistoryValue(_limiter.fps);

			//std::cout << _limiter.fps << std::endl;
			frameCounter = 0;
		}

		updateUI();

		_window.swapBuffer();
	}
}
void AppInterface::exitSimulator() {
	_currentScene->onExit();
	if (_sceneList) {
		_sceneList->destroy();
		_sceneList.reset();
	}
	_isRunning = false;
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
	if (_inputManager.isKeyDown(SDLK_ESCAPE)) {
		exitSimulator();
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
	_window.create("Taz Graph", 800, 640, 1.0f, TazGraphEngine::VISIBLE);
	return true;
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
