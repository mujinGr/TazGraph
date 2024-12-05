#include "IMainGame.h"
#include "../Timing/Timing.h"

#include "ScreenList.h"
#include "IGameScreen.h"

#include "../Camera2.5D/CameraManager.h"

IMainGame::IMainGame() {
	_screenList = std::make_unique<ScreenList>(this);
}

IMainGame::~IMainGame() {

}

void IMainGame::run() {

	const float DESIRED_FPS = 60;
	const int MAX_PHYSICS_STEPS = 6;

	if (!init()) return; // get "Game"

	const float MS_PER_SECOND = 1000;
	const float DESIRED_FRAMETIME = MS_PER_SECOND / DESIRED_FPS;
	const float MAX_DELTA_TIME = 1.0f;

	float prevTicks = SDL_GetTicks();

	FPSLimiter limiter;
	limiter.setMaxFPS(60.0f);

	_isRunning = true;
	while (_isRunning) {
		limiter.begin();

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

		_fps = limiter.end();

		static int frameCounter = 0;
		frameCounter++;
		if (frameCounter == 10) {
			std::cout << _fps << std::endl;
			frameCounter = 0;
		}

		updateUI();

		_window.swapBuffer();
	}
}
void IMainGame::exitGame() {
	_currentScreen->onExit();
	if (_screenList) {
		_screenList->destroy();
		_screenList.reset();
	}
	_isRunning = false;
}

void IMainGame::onSDLEvent(SDL_Event& evnt) {
	switch (evnt.type)
	{
	case SDL_QUIT:
		exitGame();
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
		//case SDL_MOUSEWHEEL:
		//	if (evnt.wheel.y > 0)
		//	{
		//		// Scrolling up
		//		camera2D.setScale(camera2D.getScale() + SCALE_SPEED);
		//	}
		//	else if (evnt.wheel.y < 0)
		//	{
		//		// Scrolling down
		//		camera2D.setScale(camera2D.getScale() - SCALE_SPEED);
		//	}
		//	break;
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
		exitGame();
	}
}

bool IMainGame::init() {
	//Initialize SDL
	SDL_Init(SDL_INIT_EVERYTHING);

	//Tell SDL that we want a double buffered window so we dont get
	//any flickering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	if (!initSystems()) return false;

	onInit();
	addScreens();

	CameraManager::getInstance().initializeCameras();

	_currentScreen = _screenList->getCurrent();
	_currentScreen->onEntry();
	_currentScreen->setRunning();

	return true;
}

bool IMainGame::initSystems() {
	_window.create("Taz Graph", 800, 640, 1.0f, MujinEngine::VISIBLE);
	return true;
}

void IMainGame::update(float deltaTime) {
	if (_currentScreen) {
		switch (_currentScreen->getState()) {
		case ScreenState::RUNNING:
			_currentScreen->update(deltaTime);
			break;
		case ScreenState::CHANGE_NEXT:
			_currentScreen->onExit();
			_currentScreen = _screenList->moveNext();
			if (_currentScreen) {
				_currentScreen->setRunning();
				_currentScreen->onEntry();
			}
			break;
		case ScreenState::CHANGE_PREVIOUS:
			_currentScreen->onExit();
			_currentScreen = _screenList->movePrevious();
			if (_currentScreen) {
				_currentScreen->setRunning();
				//_currentScreen->onEntry();
			}
			break;
		case ScreenState::EXIT_APPLICATION:
			exitGame();
			break;
		default:
			break;
		}
	}
	else {
		exitGame();
	}

}
void IMainGame::draw() {
	glViewport(0, 0, _window.getScreenWidth(), _window.getScreenHeight());
	if (_currentScreen && _currentScreen->getState() == ScreenState::RUNNING) {
		_currentScreen->draw();
	}
}

void IMainGame::updateUI()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	if (_currentScreen && _currentScreen->getState() == ScreenState::RUNNING) {
		_currentScreen->updateUI();
	}
	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
