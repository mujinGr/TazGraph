#include "MainMenuScreen.h"
#include <iostream>
#include <SDL/SDL.h>
#include "ECS/Components.h"
#include "../ECS/ScriptComponents.h"
#include "Camera2.5D/CameraManager.h"
#include "../ECS/ScriptComponents.h"
#include "../Collision/Collision.h"
#include "../AssetManager/AssetManager.h"
#include "GraphScreen/IMainGraph.h"

Manager main_menu_manager;

Collision main_menu_collision;

SpriteBatch MainMenuScreen::_spriteBatch;

AssetManager* MainMenuScreen::assets = nullptr;

auto& Mainmenubackground(main_menu_manager.addEntity());
auto& StartGraphButton(main_menu_manager.addEntity());
auto& ExitGraphButton(main_menu_manager.addEntity());

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
		_textureProgram.compileShaders("Src/Shaders/textureBright.vert", "Src/Shaders/textureBright.frag");
		_textureProgram.addAttribute("vertexPosition");
		_textureProgram.addAttribute("vertexColor");
		_textureProgram.addAttribute("vertexUV");
		_textureProgram.linkShaders();

		_colorProgram.compileShaders("Src/Shaders/colorShading.vert", "Src/Shaders/colorShading.frag");
		_colorProgram.addAttribute("vertexPosition");
		_colorProgram.addAttribute("vertexColor");
		_colorProgram.addAttribute("vertexUV");
		_colorProgram.linkShaders();

		MainMenuScreen::_spriteBatch.init();
	}

	if (TTF_Init() == -1)
	{
		std::cout << "Error : SDL_TTF" << std::endl;
	}

	// Texture Loads
	TextureManager::getInstance().Add_GLTexture("dungeonhall", "assets/Sprites/block_networkMiserable.png");

	TextureManager::getInstance().Add_GLTexture("startgame", "assets/Sprites/StartGame.png");
	TextureManager::getInstance().Add_GLTexture("exitgame", "assets/Sprites/ExitGame.png");

	TextureManager::getInstance().Add_GLTexture("arial", "assets/Fonts/arial_cropped_white.png");

	//main menu moving background
	Mainmenubackground.addComponent<MainMenuBackground>();

	Mainmenubackground.addGroup(Manager::groupBackgroundLayer);

	// initial entities
	glm::ivec2 mainMenuButtonsDims = glm::ivec2(200,50);
	StartGraphButton.addComponent<TransformComponent>(glm::vec2(hud_camera2D->getCameraDimensions().x / 2 - mainMenuButtonsDims.x / 2, 200.0f), Manager::actionLayer,
		mainMenuButtonsDims,
		1.0f);
	StartGraphButton.addComponent<ButtonComponent>(std::bind(&MainMenuScreen::onStartSimulator, this), "ENTER SIMULATOR", mainMenuButtonsDims, Color(120, 120, 120, 200));
	StartGraphButton.addGroup(Manager::startGraphGroup);

	ExitGraphButton.addComponent<TransformComponent>(glm::vec2(hud_camera2D->getCameraDimensions().x / 2 - mainMenuButtonsDims.x / 2, 300.0f), Manager::actionLayer,
		mainMenuButtonsDims,
		1.0f);
	ExitGraphButton.addComponent<ButtonComponent>(std::bind(&MainMenuScreen::onExitSimulator, this), "EXIT", mainMenuButtonsDims, Color(70, 70, 70, 255));
	ExitGraphButton.addGroup(Manager::exitGraphGroup);
}

auto& mainmenubackground(main_menu_manager.getGroup(Manager::groupBackgroundLayer));
auto& startgraphbuttons(main_menu_manager.getGroup(Manager::startGraphGroup));
auto& exitgraphbuttons(main_menu_manager.getGroup(Manager::exitGraphGroup));
auto& backgroundPanels(main_menu_manager.getGroup(Manager::backgroundPanels));
auto& buttonLabels(main_menu_manager.getGroup(Manager::buttonLabels));


void MainMenuScreen::onExit()
{
	for (auto& sb : startgraphbuttons)
	{
		sb->GetComponent<ButtonComponent>().setOnClick(std::bind(&MainMenuScreen::onResumeSimulator, this));
	}
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


	for (auto& sb : startgraphbuttons)
	{
		TransformComponent entityTr = sb->GetComponent<TransformComponent>();
		if (_graph->_inputManager.isKeyPressed(SDL_BUTTON_LEFT) && _graph->_inputManager.checkMouseCollision(entityTr.getPosition(), glm::ivec2(entityTr.width, entityTr.height))) { //culling
			std::cout << "clicked button" << std::endl;
			sb->GetComponent<ButtonComponent>().setState(ButtonComponent::ButtonState::PRESSED);
		}
	}
	for (auto& eb : exitgraphbuttons)
	{
		TransformComponent entityTr = eb->GetComponent<TransformComponent>();
		if (_graph->_inputManager.isKeyPressed(SDL_BUTTON_LEFT) && _graph->_inputManager.checkMouseCollision(entityTr.getPosition(), glm::ivec2(entityTr.width, entityTr.height))) { //culling
			std::cout << "clicked button" << std::endl;
			eb->GetComponent<ButtonComponent>().setState(ButtonComponent::ButtonState::PRESSED);
		}
	}
}

void MainMenuScreen::setupShaderAndTexture(const std::string& textureName) {
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("mainMenu_hud"));

	_textureProgram.use();
	glActiveTexture(GL_TEXTURE0);
	const GLTexture* texture = TextureManager::getInstance().Get_GLTexture(textureName); // can also use the sprites' textureName
	glBindTexture(GL_TEXTURE_2D, texture->id);
	GLint textureLocation = _textureProgram.getUniformLocation("texture_sampler");
	glUniform1i(textureLocation, 0);
	GLint pLocation = _textureProgram.getUniformLocation("projection");
	glm::mat4 cameraMatrix = hud_camera2D->getCameraMatrix();
	glUniformMatrix4fv(pLocation, 1, GL_FALSE, &(cameraMatrix[0][0]));
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
	glClearColor(1.0f, 1.0f, 1.f, 1.0f);
	//////////////////////////////////////

	_textureProgram.use();
	glActiveTexture(GL_TEXTURE0);
	const GLTexture* texture = TextureManager::getInstance().Get_GLTexture("dungeonhall"); // can also use the sprites' textureName
	glBindTexture(GL_TEXTURE_2D, texture->id);
	GLint textureLocation = _textureProgram.getUniformLocation("texture_sampler");
	glUniform1i(textureLocation, 0);

	GLint pLocation = _textureProgram.getUniformLocation("projection");
	glm::mat4 cameraMatrix = main_camera2D->getCameraMatrix();
	glUniformMatrix4fv(pLocation, 1, GL_FALSE, &(cameraMatrix[0][0]));

	renderBatch(mainmenubackground);

	setupShaderAndTexture("startgame");
	renderBatch(startgraphbuttons);
	setupShaderAndTexture("exitgame");
	renderBatch(exitgraphbuttons);

	_colorProgram.use();

	pLocation = _colorProgram.getUniformLocation("projection");
	cameraMatrix = hud_camera2D->getCameraMatrix();
	glUniformMatrix4fv(pLocation, 1, GL_FALSE, &(cameraMatrix[0][0]));

	renderBatch(backgroundPanels);

	_colorProgram.unuse();
	// render letters
	setupShaderAndTexture("arial");
	renderBatch(buttonLabels);

	glBindTexture(GL_TEXTURE_2D, 0);
	//drawHUD();
	_textureProgram.unuse();
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

void MainMenuScreen::updateUI() {
	// Default ImGui window
	ImGui::Begin("Default UI");
	ImGui::Text("This is a permanent UI element.");
	ImGui::End();

	ImGui::Begin("Background UI");
	ImGui::Text("This is a Background UI element.");
	// Change color based on the debug mode state

	ImGui::Text("Camera Position");

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("mainMenu_main"));

	ImGui::Text("Rect: {x: %d, y: %d, w: %d, h: %d}", main_camera2D->getCameraRect().x, main_camera2D->getCameraRect().y, main_camera2D->getCameraRect().w, main_camera2D->getCameraRect().h);

	if (ImGui::SliderFloat3("Eye Position", &main_camera2D->eyePos[0], -1000.0f, 1000.0f)) {
		main_camera2D->setCameraMatrix(glm::lookAt(main_camera2D->eyePos, main_camera2D->aimPos, main_camera2D->upDir));
	}
	if (ImGui::SliderFloat3("Aim Position", &main_camera2D->aimPos[0], -1000.0f, 1000.0f)) {
		main_camera2D->setCameraMatrix(glm::lookAt(main_camera2D->eyePos, main_camera2D->aimPos, main_camera2D->upDir));
	}
	if (ImGui::SliderFloat3("Up Direction", &main_camera2D->upDir[0], -1000.0f, 1000.0f)) {
		main_camera2D->setCameraMatrix(glm::lookAt(main_camera2D->eyePos, main_camera2D->aimPos, main_camera2D->upDir));
	}

	ImGui::Text("Mouse Coords: {x: %f, y: %f}", _graph->_inputManager.getMouseCoords().x, _graph->_inputManager.getMouseCoords().y);

	ImGui::End();
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