#pragma once

#include "../DebugRenderer/DebugRenderer.h"

#define SCREEN_INDEX_NO_SCREEN -1

class IMainGame;

enum class ScreenState {
	NONE,
	RUNNING,
	EXIT_APPLICATION,
	CHANGE_NEXT,
	CHANGE_PREVIOUS
};

class IGameScreen {
public:
	friend class ScreenList;
	IGameScreen() {

	}
	virtual ~IGameScreen() {

	}

	//Returns the index of the next or previous screen when changing screens
	virtual int getNextScreenIndex() const = 0;
	virtual int getPreviousScreenIndex() const = 0;

	//Called at beginning and end of application
	virtual void build() = 0;
	virtual void destroy() = 0;

	//Called when a screen enters and exits focus
	virtual void onEntry() = 0;
	virtual void onExit() = 0;

	virtual void update(float deltaTime) = 0;
	virtual void draw() = 0;

	virtual void updateUI() = 0;

	int getScreenIndex() const {
		return _screenIndex;
	}
	void setRunning() {
		_currentState = ScreenState::RUNNING;
	}

	ScreenState getState() const { return _currentState; }

	void setParentGame(IMainGame* game) { _game = game; }

protected:
	ScreenState _currentState = ScreenState::NONE;
	IMainGame* _game = nullptr;
	int _screenIndex = -1;

	DebugRenderer _debugRenderer;

	bool _renderDebug = false;
};