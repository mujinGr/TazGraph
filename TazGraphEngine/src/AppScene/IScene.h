#pragma once

#include "./ScreenIndices.h"
#include "../DataManager/DataManager.h"

#include "../GECS/Core/GECSManager.h"

#define SCENE_INDEX_NO_SCENE -1

class AppInterface;

enum class SceneState {
	NONE,
	RUNNING,
	EXIT_APPLICATION,
	CHANGE_NEXT,
	CHANGE_PREVIOUS
};

#define ON_HOVER 0
#define CTRLD_LEFT_CLICK -1
#define HOLD_TIME_FOR_SELECTION 1000
#define SELECT_DISTANCE 300000.0f

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

	virtual void checkInput() = 0;

	virtual void update(float deltaTime) = 0;
	virtual void prepareDraw() = 0;
	virtual void renderDraw() = 0;

	virtual void SwapBufferDraw() = 0;

	virtual void BeginRender() = 0;
	virtual void updateUI(float deltaTime) = 0;
	virtual void drawUI() = 0;
	virtual void EndRender() = 0;

	int getSceneIndex() const {
		return _sceneIndex;
	}
	void setRunning() {
		currentState = SceneState::RUNNING;
	}

	SceneState getState() const { return currentState; }

	void setParentApp(AppInterface* app) { _app = app; }

	AppInterface* getApp() const { return _app; }

	virtual bool setManager(std::string m_managerName) {
		if (!m_managerName.empty()) {
			auto it = managers.find(m_managerName);
			if (it == managers.end()) {
				managers[m_managerName] = new Manager();
			}
			manager = managers[m_managerName];
			managerName = m_managerName;
		}
		return false;
	};

	std::unordered_map<std::string, Manager*> managers = {
	};

	Manager* manager = nullptr;
	std::string managerName = "";

	bool last_renderDebug = false;
	bool renderDebug = false;

	bool showGrid = true;
	bool last_showGrid = false;

	SceneState currentState = SceneState::NONE;

	float backgroundColor[4] = { 0.8f, 0.8f, 0.8f, 1.0f };

	bool requestExit = false;

	Taz::FrameRenderData frameDataBuffers[2];
	Taz::FrameRenderData minimap_frameDataBuffers[2];
	std::atomic<int> activeFrameIndex = 0;

protected:
	AppInterface* _app = nullptr;
	int _sceneIndex = -1;

};