#pragma once

#include "../Renderers/LineRenderer/LineRenderer.h"

#define SCREEN_INDEX_NO_SCREEN -1

class IMainGraph;

enum class ScreenState {
	NONE,
	RUNNING,
	EXIT_APPLICATION,
	CHANGE_NEXT,
	CHANGE_PREVIOUS
};

class IGraphScreen {
public:
	friend class ScreenList;
	IGraphScreen() {

	}
	virtual ~IGraphScreen() {

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

	virtual void BeginRender() = 0;
	virtual void updateUI() = 0;
	virtual void EndRender() = 0;

	int getScreenIndex() const {
		return _screenIndex;
	}
	void setRunning() {
		_currentState = ScreenState::RUNNING;
	}

	ScreenState getState() const { return _currentState; }

	void setParentGraph(IMainGraph* graph) { _graph = graph; }

	IMainGraph* getInterfaceGraph() const { return _graph; }
protected:
	ScreenState _currentState = ScreenState::NONE;
	IMainGraph* _graph = nullptr;
	int _screenIndex = -1;

	LineRenderer _LineRenderer;

	bool _renderDebug = false;
};