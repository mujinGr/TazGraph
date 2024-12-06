#include "App.h"
#include "GraphScreen/ScreenList.h"

App::App() {

}

App::~App() {

}

void App::onInit() {

}

void App::addScreens() {
    _mainMenuScreen = std::make_unique<MainMenuScreen>(&_window);
    _graphplayScreen = std::make_unique<Graph>(&_window);
    //m_editorScreen = std::make_unique<EditorScreen>(&_window);

    _screenList->addScreen(_mainMenuScreen.get());
    _screenList->addScreen(_graphplayScreen.get());
    //m_screenList->addScreen(_editorScreen.get());

    _screenList->setScreen(_mainMenuScreen->getScreenIndex());
}

void App::onExit() {

}