#include "App.h"
#include "GraphScreen/SceneList.h"

App::App() {

}

App::~App() {

}

void App::onInit() {

}

void App::addScenes() {
    _mainMenuScreen = std::make_unique<MainMenuScreen>(&_window);
    _graphplayScreen = std::make_unique<Graph>(&_window);
    //m_editorScreen = std::make_unique<EditorScreen>(&_window);

    _sceneList->addScene("main_menu", _mainMenuScreen.get());
    _sceneList->addScene(_graphplayScreen.get());
    //m_screenList->addScreen(_editorScreen.get());

    _sceneList->setScene(_mainMenuScreen->getSceneIndex());
}

void App::onExit() {

}