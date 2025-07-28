#include "MainMenuScreen.h"
#include <iostream>
#include <SDL2/SDL.h>
#include "GECS/Components.h"
#include "../GECS/ScriptComponents.h"
#include "Camera2.5D/CameraManager.h"
#include "../GECS/ScriptComponents.h"
#include "../AssetManager/AssetManager.h"
#include "GraphScreen/AppInterface.h"



MainMenuScreen::MainMenuScreen(TazGraphEngine::Window* window)
	: _window(window)
{
	_sceneIndex = SCENE_INDEX_MAIN_MENU;
}

MainMenuScreen::~MainMenuScreen()
{
	//dtor
}

int MainMenuScreen::getNextSceneIndex() const
{
	return _nextSceneIndex;
}

int MainMenuScreen::getPreviousSceneIndex() const
{
	return _prevSceneIndex;
}

void MainMenuScreen::build()
{

}

void MainMenuScreen::destroy()
{

}

void MainMenuScreen::onEntry()
{
	auto& Mainmenubackground(manager->addEntity<Empty>());

	_resourceManager.addGLSLProgram("texture");
	_resourceManager.addGLSLProgram("color");

	_assetsManager = new AssetManager(manager, _app->_inputManager, _app->_window);

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("mainMenu_main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("mainMenu_hud"));

	main_camera2D->init(); // Assuming a screen resolution of 800x600
	main_camera2D->setPosition_X(main_camera2D->getPosition().x /*+ glm::vec2(
		width / 2.0f,
		height / 2.0f
	)*/);
	main_camera2D->setPosition_Y(main_camera2D->getPosition().y);
	main_camera2D->setScale(1.0f);

	hud_camera2D->init(); // Assuming a screen resolution of 800x600
	hud_camera2D->setPosition_X(hud_camera2D->getPosition().x /*+ glm::vec2(
		width / 2.0f,
		height / 2.0f
	)*/);
	hud_camera2D->setPosition_Y(hud_camera2D->getPosition().y);
	hud_camera2D->setScale(1.0f);

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		std::cout << "Subsystems Initialised..." << std::endl;

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);


		//InitShaders function from Bengine
		_resourceManager.getGLSLProgram("texture")->compileAndLinkShaders("Src/Shaders/textureBright.vert", "Src/Shaders/textureBright.frag");
		_resourceManager.getGLSLProgram("texture")->addAttribute("vertexPosition");
		_resourceManager.getGLSLProgram("texture")->addAttribute("vertexColor");
		_resourceManager.getGLSLProgram("texture")->addAttribute("vertexUV");

		_resourceManager.getGLSLProgram("color")->compileAndLinkShaders("Src/Shaders/colorShading.vert", "Src/Shaders/colorShading.frag");
		_resourceManager.getGLSLProgram("color")->addAttribute("vertexPosition");
		_resourceManager.getGLSLProgram("color")->addAttribute("vertexColor");
		_resourceManager.getGLSLProgram("color")->addAttribute("vertexUV");

		MainMenuScreen::_PlaneModelRenderer.init();
	}

	if (TTF_Init() == -1)
	{
		std::cout << "Error : SDL_TTF" << std::endl;
	}

	// Texture Loads
	TextureManager::getInstance().Add_GLTexture("graphnetwork", "assets/Sprites/block_networkMiserable.png");
	TextureManager::getInstance().Add_GLTexture("arial", "assets/Fonts/arial_cropped_white.png");

	if (!manager->grid)
	{
		manager->grid = std::make_unique<Grid>(ROW_CELL_SIZE, COLUMN_CELL_SIZE, DEPTH_CELL_SIZE, CELL_SIZE);

		Mainmenubackground.addComponent<MainMenuBackground>();
		Mainmenubackground.addGroup(Manager::groupBackgroundLayer);
		manager->grid->addEmpty(&Mainmenubackground, manager->grid->getGridLevel());
	}
}



void MainMenuScreen::onExit()
{
	
}

void MainMenuScreen::update(float deltaTime)
{
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("mainMenu_main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("mainMenu_hud"));

	manager->update(deltaTime);
	manager->refresh(main_camera2D.get());

	main_camera2D->update();
	hud_camera2D->update();
}

void MainMenuScreen::renderBatch(const std::vector<EmptyEntity*>& entities) {
	for (const auto& entity : entities) {
		entity->GetComponent<SpriteComponent>().draw(0, _PlaneModelRenderer, *Graph::_window);
	}

}

void MainMenuScreen::draw()
{
	auto& mainmenubackground(manager->getGroup<EmptyEntity>(Manager::groupBackgroundLayer));

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("mainMenu_main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("mainMenu_hud"));

	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(_backgroundColor[0], _backgroundColor[1], _backgroundColor[2], _backgroundColor[3]);
	//////////////////////////////////////


	_PlaneModelRenderer.begin();

	_PlaneModelRenderer.initQuadBatch(mainmenubackground.size());

	_PlaneModelRenderer.initBatchSize();


	renderBatch(mainmenubackground);

	_resourceManager.setupShader(*_resourceManager.getGLSLProgram("texture"), *main_camera2D);
	
	_PlaneModelRenderer.end();
	_PlaneModelRenderer.renderBatch(_resourceManager.getGLSLProgram("texture"));
	
	glBindTexture(GL_TEXTURE_2D, 0);
	//drawHUD();
	_resourceManager.getGLSLProgram("texture")->unuse();
}

void MainMenuScreen::checkInput() {
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	SDL_Event evnt;

	while (SDL_PollEvent(&evnt)) {
		ImGui_ImplSDL2_ProcessEvent(&evnt);
		_app->onSDLEvent(evnt);

		switch (evnt.type)
		{
			case SDL_MOUSEMOTION:
				glm::vec2 mouseCoordsVec = _app->_inputManager.getMouseCoords();
				_app->_inputManager.setMouseCoords(mouseCoordsVec.x * main_camera2D->getCameraDimensions().x / _window->getScreenWidth(), mouseCoordsVec.y * main_camera2D->getCameraDimensions().y / _window->getScreenHeight());
		}

		if (_app->_inputManager.isKeyPressed(SDL_BUTTON_LEFT)) {
			glm::vec2 mouseCoordsVec = _app->_inputManager.getMouseCoords();
			std::cout << mouseCoordsVec.x << " " << mouseCoordsVec.y << std::endl;
		}


	}
}

void MainMenuScreen::BeginRender() {
	_editorImgui.BeginRender();
}

void MainMenuScreen::updateUI() {
	_editorImgui.MainMenuUI(
		[this]() { MainMenuScreen::onStartSimulator(); },
		[this]() { MainMenuScreen::onLoadSimulator(); },
		[this]() { MainMenuScreen::onExitSimulator(); }
		);

	if (_editorImgui.isLoading()) {
		char* loadMapPath = _editorImgui.LoadingUI();
		if (!_editorImgui.isLoading() && !std::string(loadMapPath).empty()) {
			DataManager::getInstance().mapToLoad = loadMapPath;
			_nextSceneIndex = SCENE_INDEX_GRAPHPLAY;
			_currentState = SceneState::CHANGE_NEXT;
		}
	}
}

void MainMenuScreen::EndRender() {
	_editorImgui.EndRender();
}

bool MainMenuScreen::onStartSimulator() {
	_nextSceneIndex = SCENE_INDEX_GRAPHPLAY;
	_currentState = SceneState::CHANGE_NEXT;
	return true;
}

bool MainMenuScreen::onResumeSimulator() {
	_prevSceneIndex = SCENE_INDEX_GRAPHPLAY;
	_currentState = SceneState::CHANGE_PREVIOUS;
	return true;
}

bool MainMenuScreen::onLoadSimulator() {
	_editorImgui.setLoading(true);
	return true;
}

void MainMenuScreen::onExitSimulator() {
	_app->exitSimulator();
}