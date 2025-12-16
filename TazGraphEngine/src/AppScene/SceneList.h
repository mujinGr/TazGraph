#pragma once

#include "../../pch.h"

#include "./IScene.h"

class AppInterface;

class SceneList {
public:
	SceneList(AppInterface* app);
	~SceneList();

	IScene* moveNext();
	IScene* movePrevious();

	void setScene(int nextScene);
	void addScene(IScene* newScene);
	void addScene(std::string managerName, IScene* newScene);

	void destroy();

	IScene* getCurrent();

protected:
	AppInterface* _app = nullptr;
	std::vector<IScene*> _scenes;
	int _currentSceneIndex = -1;
};