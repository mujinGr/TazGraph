#pragma once

#include "../Renderers/LineRenderer/LineRenderer.h"
#include "../DataManager/DataManager.h"

#define SCENE_INDEX_NO_SCENE -1

class AppInterface;

enum class SceneState {
	NONE,
	RUNNING,
	EXIT_APPLICATION,
	CHANGE_NEXT,
	CHANGE_PREVIOUS
};

class IScene {
public:
	friend class SceneList;
	IScene() {

	}
	virtual ~IScene() {

	}

	//Returns the index of the next or previous screen when changing screens
	virtual int getNextSceneIndex() const = 0;
	virtual int getPreviousSceneIndex() const = 0;

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

	int getSceneIndex() const {
		return _sceneIndex;
	}
	void setRunning() {
		_currentState = SceneState::RUNNING;
	}

	SceneState getState() const { return _currentState; }

	void setParentApp(AppInterface* app) { _app = app; }

	AppInterface* getApp() const { return _app; }
protected:
	SceneState _currentState = SceneState::NONE;
	AppInterface* _app = nullptr;
	int _sceneIndex = -1;


	bool _renderDebug = false;
};