#include "MainMenuScreen.h"
#include <iostream>
#include <SDL2/SDL.h>
#include "GOS/Components.h"
#include "../GOS/ScriptComponents.h"
#include "Camera2.5D/CameraManager.h"
#include "../GOS/ScriptComponents.h"
#include "../Collision/Collision.h"
#include "../AssetManager/AssetManager.h"
#include "GraphScreen/IMainGraph.h"

Manager main_menu_manager;

Collision main_menu_collision;

SpriteBatch MainMenuScreen::_spriteBatch;

AssetManager* MainMenuScreen::assets = nullptr;

float MainMenuScreen::backgroundColor[4] = { 0.8f, 0.8f, 0.8f, 1.0f };

auto& Mainmenubackground(main_menu_manager.addEntity());

MainMenuScreen::MainMenuScreen(TazGraphEngine::Window* window)
	: _window(window)
{
	_screenIndex = SCREEN_INDEX_MAIN_MENU;
}

MainMenuScreen::~MainMenuScreen()
{
	//dtor
}

int MainMenuScreen::getNextScreenIndex() const
{
	return _nextScreenIndex;
}

int MainMenuScreen::getPreviousScreenIndex() const
{
	return _prevScreenIndex;
}

void MainMenuScreen::build()
{

}

void MainMenuScreen::destroy()
{

}

void MainMenuScreen::onEntry()
{
	_resourceManager.addGLSLProgram("texture");
	_resourceManager.addGLSLProgram("color");


	assets = new AssetManager(&manager, _graph->_inputManager, _graph->_window);

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
		_resourceManager.getGLSLProgram("texture")->compileShaders("Src/Shaders/textureBright.vert", "Src/Shaders/textureBright.frag");
		_resourceManager.getGLSLProgram("texture")->addAttribute("vertexPosition");
		_resourceManager.getGLSLProgram("texture")->addAttribute("vertexColor");
		_resourceManager.getGLSLProgram("texture")->addAttribute("vertexUV");
		_resourceManager.getGLSLProgram("texture")->linkShaders();

		_resourceManager.getGLSLProgram("color")->compileShaders("Src/Shaders/colorShading.vert", "Src/Shaders/colorShading.frag");
		_resourceManager.getGLSLProgram("color")->addAttribute("vertexPosition");
		_resourceManager.getGLSLProgram("color")->addAttribute("vertexColor");
		_resourceManager.getGLSLProgram("color")->addAttribute("vertexUV");
		_resourceManager.getGLSLProgram("color")->linkShaders();

		MainMenuScreen::_spriteBatch.init();
	}

	if (TTF_Init() == -1)
	{
		std::cout << "Error : SDL_TTF" << std::endl;
	}

	// Texture Loads
	TextureManager::getInstance().Add_GLTexture("graphnetwork", "assets/Sprites/block_networkMiserable.png");
	TextureManager::getInstance().Add_GLTexture("arial", "assets/Fonts/arial_cropped_white.png");
	//main menu moving background
	Mainmenubackground.addComponent<MainMenuBackground>();
	Mainmenubackground.addGroup(Manager::groupBackgroundLayer);
}

auto& mainmenubackground(main_menu_manager.getGroup(Manager::groupBackgroundLayer));


void MainMenuScreen::onExit()
{
	
}



void MainMenuScreen::update(float deltaTime)
{
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("mainMenu_main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("mainMenu_hud"));

	checkInput();

	main_menu_manager.refresh();
	main_menu_manager.update(deltaTime);

	main_camera2D->update();
	hud_camera2D->update();
}

void MainMenuScreen::renderBatch(const std::vector<Entity*>& entities) {
	_spriteBatch.begin();
	for (const auto& entity : entities) {
		entity->draw(_spriteBatch, *Graph::_window);
	}
	_spriteBatch.end();
	_spriteBatch.renderBatch();
}

void MainMenuScreen::draw()
{
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("mainMenu_main"));

	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("mainMenu_hud"));

	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
	//////////////////////////////////////

	_resourceManager.setupShader(*_resourceManager.getGLSLProgram("texture"), "graphnetwork", *main_camera2D);
	renderBatch(mainmenubackground);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	//drawHUD();
	_resourceManager.getGLSLProgram("texture")->unuse();
}

void MainMenuScreen::checkInput() {
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	SDL_Event evnt;

	_graph->_inputManager.update();

	while (SDL_PollEvent(&evnt)) {
		ImGui_ImplSDL2_ProcessEvent(&evnt);
		_graph->onSDLEvent(evnt);

		switch (evnt.type)
		{
			case SDL_MOUSEMOTION:
				glm::vec2 mouseCoordsVec = _graph->_inputManager.getMouseCoords();
				_graph->_inputManager.setMouseCoords(mouseCoordsVec.x * main_camera2D->getCameraDimensions().x / _window->getScreenWidth(), mouseCoordsVec.y * main_camera2D->getCameraDimensions().y / _window->getScreenHeight());
		}

		if (_graph->_inputManager.isKeyPressed(SDL_BUTTON_LEFT)) {
			glm::vec2 mouseCoordsVec = _graph->_inputManager.getMouseCoords();
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
		[this]() { MainMenuScreen::onExitSimulator(); }
		);
}

void MainMenuScreen::EndRender() {
	_editorImgui.EndRender();
}

bool MainMenuScreen::onStartSimulator() {
	_nextScreenIndex = SCREEN_INDEX_GRAPHPLAY;
	_currentState = ScreenState::CHANGE_NEXT;
	return true;
}

bool MainMenuScreen::onResumeSimulator() {
	_prevScreenIndex = SCREEN_INDEX_GRAPHPLAY;
	_currentState = ScreenState::CHANGE_PREVIOUS;
	return true;
}

void MainMenuScreen::onExitSimulator() {
	_graph->exitSimulator();
}