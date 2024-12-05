#include "App.h"
#include "GameScreen/ScreenList.h"

App::App() {

}

App::~App() {

}

void App::onInit() {

}

void App::addScreens() {
    _mainMenuScreen = std::make_unique<MainMenuScreen>(&_window);
    _gameplayScreen = std::make_unique<Game>(&_window);
    //m_editorScreen = std::make_unique<EditorScreen>(&_window);

    _screenList->addScreen(_mainMenuScreen.get());
    _screenList->addScreen(_gameplayScreen.get());
    //m_screenList->addScreen(_editorScreen.get());

    _screenList->setScreen(_mainMenuScreen->getScreenIndex());
}

void App::onExit() {

}