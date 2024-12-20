#pragma once

#include "../InputManager/InputManager.h"
#include "../Window/Window.h"
#include <memory>
#include <iostream>

#include "ScreenList.h"
#include "../BaseFPSLimiter/BaseFPSLimiter.h"

class ScreenList;
class IGraphScreen;

class IMainGraph {
public:
	IMainGraph();
	virtual ~IMainGraph();

	void run();
	void exitSimulator();

	virtual void onInit() = 0;
	virtual void addScreens() = 0;
	virtual void onExit() = 0;

	void onSDLEvent(SDL_Event& evnt);

	InputManager _inputManager;
	TazGraphEngine::Window _window;

	BaseFPSLimiter getFPSLimiter() const { return _limiter; }

protected:
	virtual void update(float deltaTime);
	virtual void draw();
	virtual void updateUI();

	bool init();
	bool initSystems();

	BaseFPSLimiter _limiter;

	std::unique_ptr<ScreenList> _screenList = nullptr;
	IGraphScreen* _currentScreen = nullptr;
	bool _isRunning = false;

	const float SCALE_SPEED = 0.1f;
};