#include "SceneList.h"
#include "IScene.h"

SceneList::SceneList(AppInterface* app) :
	_app(app) {

}
SceneList::~SceneList() {
	destroy();
}

IScene* SceneList::moveNext() {
	IScene* currentScene = getCurrent();
	if (currentScene->getNextSceneIndex() != SCENE_INDEX_NO_SCENE) {
		_currentSceneIndex = currentScene->getNextSceneIndex();
	}
	return getCurrent();
}
IScene* SceneList::movePrevious() {
	IScene* currentScreen = getCurrent();
	if (currentScreen->getPreviousSceneIndex() != SCENE_INDEX_NO_SCENE) {
		_currentSceneIndex = currentScreen->getPreviousSceneIndex();
	}
	return getCurrent();
}

void SceneList::setScene(int nextScene) {
	_currentSceneIndex = nextScene;
}
void SceneList::addScene(IScene* newScene) {
	newScene->_sceneIndex = static_cast<int>(_scenes.size());
	_scenes.push_back(newScene);
	newScene->build();
	newScene->setParentApp(_app);
}

void SceneList::addScene(std::string managerName, IScene* newScene) {
	addScene(newScene);
	// add manager to scene with name: managerName
	newScene->managers[managerName] = new Manager();
	newScene->manager = newScene->managers[managerName];
}

void SceneList::destroy() {
	for (size_t i = 0; i < _scenes.size(); i++) {
		_scenes[i]->destroy();
	}
	_scenes.resize(0);
	_currentSceneIndex = SCENE_INDEX_NO_SCENE;
}

IScene* SceneList::getCurrent() {
	if (_currentSceneIndex == SCENE_INDEX_NO_SCENE) return nullptr;

	return _scenes[_currentSceneIndex];
}