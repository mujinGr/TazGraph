#pragma once

#include "../InputManager/InputManager.h"
#include "../Window/Window.h"
#include <memory>
#include <iostream>

class ScreenList;
class IGameScreen;

class IMainGame {
public:
	IMainGame();
	virtual ~IMainGame();

	void run();
	void exitGame();

	virtual void onInit() = 0;
	virtual void addScreens() = 0;
	virtual void onExit() = 0;

	void onSDLEvent(SDL_Event& evnt);

	const float getFps() const {
		return _fps;
	}

	InputManager _inputManager;
	MujinEngine::Window _window;

protected:
	virtual void update(float deltaTime);
	virtual void draw();
	virtual void updateUI();

	bool init();
	bool initSystems();

	std::unique_ptr<ScreenList> _screenList = nullptr;
	IGameScreen* _currentScreen = nullptr;
	bool _isRunning = false;
	float _fps = 0.0f;

	const float SCALE_SPEED = 0.1f;
};