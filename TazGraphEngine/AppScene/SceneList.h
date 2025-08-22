#pragma once
#include <vector>
#include <string>

class AppInterface;
class IScene;

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