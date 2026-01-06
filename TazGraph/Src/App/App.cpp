#include "App.h"

App::App(
	int threadCount,
	int msaa_samples,
	std::string openFile,
	double initialTimestamp,
	int initialStep,
	bool usePython,
	std::array<float, 4> bg,
	bool useGrid) : AppInterface(threadCount, msaa_samples, openFile,
		initialTimestamp,
		initialStep,
		usePython,
		bg, useGrid) {

}

App::~App() {

}

void App::onInit() {

}

void App::addScenes() {
	_mainMenuScreen = std::make_unique<MainMenuScreen>();
	_graphplayScreen = std::make_unique<Graph>(usePython, backgroundColor, useGrid);
	//m_editorScreen = std::make_unique<EditorScreen>(&_window);

	_sceneList->addScene("main_menu", _mainMenuScreen.get());
	_sceneList->addScene(_graphplayScreen.get());
	//m_screenList->addScreen(_editorScreen.get());

	_sceneList->setScene(_mainMenuScreen->getSceneIndex());
}

void App::onExit() {

}