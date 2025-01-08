#pragma once

#include "../InputManager/InputManager.h"
#include "../Window/Window.h"
#include <memory>
#include <iostream>

#include "SceneList.h"
#include "../BaseFPSLimiter/BaseFPSLimiter.h"

#include "../AudioEngine/AudioEngine.h"

class SceneList;
class IScene;

class AppInterface {
public:
	AppInterface();
	virtual ~AppInterface();

	void run();
	void exitSimulator();

	virtual void onInit() = 0;
	virtual void addScenes() = 0;
	virtual void onExit() = 0;

	void onSDLEvent(SDL_Event& evnt);

	InputManager _inputManager;
	TazGraphEngine::Window _window;

	BaseFPSLimiter& getFPSLimiter() { return _limiter; }
	AudioEngine& getAudioEngine() { return _audioEngine; }

protected:
	virtual void update(float deltaTime);
	virtual void draw();
	virtual void updateUI();

	bool init();
	bool initSystems();

	BaseFPSLimiter _limiter;
	AudioEngine _audioEngine;

	std::unique_ptr<SceneList> _sceneList = nullptr;
	IScene* _currentScene = nullptr;
	bool _isRunning = false;

	const float SCALE_SPEED = 0.1f;
};