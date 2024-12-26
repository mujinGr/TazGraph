
#include "Graph.h"
#include "TextureManager/TextureManager.h"
#include "Camera2.5D/CameraManager.h"
#include "Map/Map.h"
#include "GOS/Components.h"
#include "GOS/ScriptComponents.h"
#include "Collision/Collision.h"
#include "Map/Map.h"
#include "AssetManager/AssetManager.h"
#include "SceneManager/SceneManager.h"
#include <sstream>
#include "GraphScreen/IMainGraph.h"

#undef main

SDL_Event Graph::event;
Manager manager;
Collision collision;

SpriteBatch Graph::_spriteBatch;
SpriteBatch Graph::_hudSpriteBatch;

AudioEngine Graph::audioEngine;

Map* Graph::map = nullptr;
AssetManager* Graph::assets = nullptr;
SceneManager* Graph::scenes = new SceneManager();
float Graph::backgroundColor[4] = { 0.78f,0.88f,1.f, 1.0f};
TazGraphEngine::Window* Graph::_window = nullptr;

auto& cursor(manager.addEntity());

auto& nodeslabel(manager.addEntity(true));

Graph::Graph(TazGraphEngine::Window* window)
{
	_window = window;
	_screenIndex = SCREEN_INDEX_GRAPHPLAY;
}
Graph::~Graph()
{

}

int Graph::getNextScreenIndex() const {
	return _nextScreenIndex;
}

int Graph::getPreviousScreenIndex() const {
	return _prevScreenIndex;
}

void Graph::build() {

}

void Graph::destroy() {

}

void Graph::onEntry()
{
	_resourceManager.addGLSLProgram("color");
	_resourceManager.addGLSLProgram("circleColor");
	_resourceManager.addGLSLProgram("texture");


	assets = new AssetManager(&manager, _graph->_inputManager, _graph->_window);

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	_debugRenderer.init();

	main_camera2D->init(); // Assuming a screen resolution of 800x600
	main_camera2D->setPosition_X(main_camera2D->getPosition().x /*+ glm::vec2(
		width / 2.0f,
		height / 2.0f
	)*/);
	main_camera2D->setPosition_Y(main_camera2D->getPosition().y);
	main_camera2D->setScale(1.0f);

	hud_camera2D->init();

	audioEngine.init();
	AnimatorManager& animManager = AnimatorManager::getInstance();
	animManager.InitializeAnimators();
	
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		std::cout << "Subsystems Initialised..." << std::endl;

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		_resourceManager.getGLSLProgram("color")->compileShaders("Src/Shaders/colorShading.vert", "Src/Shaders/colorShading.frag");
		_resourceManager.getGLSLProgram("color")->addAttribute("vertexPosition");
		_resourceManager.getGLSLProgram("color")->addAttribute("vertexColor");
		_resourceManager.getGLSLProgram("color")->addAttribute("vertexUV");
		_resourceManager.getGLSLProgram("color")->linkShaders();

		_resourceManager.getGLSLProgram("circleColor")->compileShaders("Src/Shaders/circleColorShading.vert", "Src/Shaders/circleColorShading.frag");
		_resourceManager.getGLSLProgram("circleColor")->addAttribute("vertexPosition");
		_resourceManager.getGLSLProgram("circleColor")->addAttribute("vertexColor");
		_resourceManager.getGLSLProgram("circleColor")->addAttribute("vertexUV");
		_resourceManager.getGLSLProgram("circleColor")->linkShaders();

		_resourceManager.getGLSLProgram("texture")->compileShaders("Src/Shaders/textureBright.vert", "Src/Shaders/textureBright.frag");
		_resourceManager.getGLSLProgram("texture")->addAttribute("vertexPosition");
		_resourceManager.getGLSLProgram("texture")->addAttribute("vertexColor");
		_resourceManager.getGLSLProgram("texture")->addAttribute("vertexUV");
		_resourceManager.getGLSLProgram("texture")->linkShaders();

		Graph::_spriteBatch.init();
		Graph::_hudSpriteBatch.init();
	}

	if (TTF_Init() == -1)
	{
		std::cout << "Error : SDL_TTF" << std::endl;
	}

	//add the textures to our texture library
	TextureManager::getInstance().Add_GLTexture("arial", "assets/Fonts/arial_cropped_white.png");

	Graph::map = new Map("terrain", 1, 32);

	main_camera2D->worldDimensions= map->GetLayerDimensions("assets/Maps/map_v3_Tile_Layer.csv");

	manager.grid = std::make_unique<Grid>(main_camera2D->worldDimensions.x, main_camera2D->worldDimensions.y, CELL_SIZE);

	map->LoadMap("assets/Maps/map_v3_Tile_Layer.csv");

	assets->CreateCursor(cursor);

	nodeslabel.addComponent<TransformComponent>(glm::vec2(32, 608), Manager::actionLayer, glm::ivec2(32, 32), 1);
	nodeslabel.addComponent<UILabel>(&manager, "total nodes: 0", "arial");
	nodeslabel.addGroup(Manager::groupLabels);

	Music music = audioEngine.loadMusic("Sounds/JPEGSnow.ogg");
	music.play(-1);
}

void Graph::onExit() {
	_debugRenderer.dispose();
}

auto& nodes(manager.getGroup(Manager::groupActionLayer));
auto& colliders(manager.getGroup(Manager::groupColliders));
auto& labels(manager.getGroup(Manager::groupLabels));

auto& cursors(manager.getGroup(Manager::cursorGroup));

glm::vec2 convertScreenToWorld(glm::vec2 screenCoords) {
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	return main_camera2D->convertScreenToWorld(screenCoords);
}

void Graph::update(float deltaTime) //game objects updating
{
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	glm::vec2 mouseCoordsVec = _graph->_inputManager.getMouseCoords();

	checkInput(); //handleEvents

	main_camera2D->update();
	hud_camera2D->update();

	manager.refresh();
	if (_firstLoop) {
		manager.updateFully(deltaTime);
		_firstLoop = false;
	}
	else {
		manager.update(deltaTime);
	}

	for (auto& cursor : cursors) {
		cursor->GetComponent<TransformComponent>().setPosition_X(convertScreenToWorld(mouseCoordsVec).x);
		cursor->GetComponent<TransformComponent>().setPosition_Y(convertScreenToWorld(mouseCoordsVec).y);

		TransformComponent* transform = &cursor->GetComponent<TransformComponent>();
		cursor->GetComponent<TransformComponent>().setPosition_X(transform->getPosition().x - transform->getSizeCenter().x);
		cursor->GetComponent<TransformComponent>().setPosition_Y(transform->getPosition().y - transform->getSizeCenter().y);

	}
	// make camera not being able to move out of bounds
	//collision.moveFromOuterBounds(*pl, *_window);
}



void Graph::selectEntityAtPosition(glm::vec2 worldCoords) {
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	for (auto& groups : { nodes})
	{
		for (auto& entity : groups) {
			TransformComponent* tr = &entity->GetComponent<TransformComponent>();
			glm::vec2 pos = glm::vec2(tr->getPosition().x, tr->getPosition().y);
			// Check if the mouse click is within the entity's bounding box
			if (worldCoords.x > pos.x && worldCoords.x < pos.x + tr->width &&
				worldCoords.y > pos.y && worldCoords.y < pos.y + tr->height) {
				_selectedEntity = entity;  // Store a pointer or reference to the selected entity
				break;
			}
		}
	}
	
}

void Graph::checkInput() {
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	_graph->_inputManager.update();

	SDL_Event evnt;
	while (SDL_PollEvent(&evnt)) {
		ImGui_ImplSDL2_ProcessEvent(&evnt);
		_graph->onSDLEvent(evnt);

		switch (evnt.type)
		{
		case SDL_MOUSEWHEEL:
			if (evnt.wheel.y > 0)
			{
				// Scrolling up
				main_camera2D->setScale(main_camera2D->getScale() + SCALE_SPEED);
			}
			else if (evnt.wheel.y < 0)
			{
				// Scrolling down
				main_camera2D->setScale(main_camera2D->getScale() - SCALE_SPEED);
			}
			break;
		case SDL_MOUSEMOTION:
			glm::vec2 mouseCoordsVec = _graph->_inputManager.getMouseCoords();
			_graph->_inputManager.setMouseCoords(mouseCoordsVec.x * main_camera2D->getCameraDimensions().x / _window->getScreenWidth(), mouseCoordsVec.y * main_camera2D->getCameraDimensions().y / _window->getScreenHeight());
			mouseCoordsVec = _graph->_inputManager.getMouseCoords();
			if (_selectedEntity) {
				_selectedEntity->GetComponent<TransformComponent>().setPosition_X(convertScreenToWorld(mouseCoordsVec).x);
				_selectedEntity->GetComponent<TransformComponent>().setPosition_Y(convertScreenToWorld(mouseCoordsVec).y);
				if (_selectedEntity->hasComponent<GridComponent>())
					_selectedEntity->GetComponent<GridComponent>().updateCollidersGrid();
			}

			if (_graph->_inputManager.isKeyDown(SDL_BUTTON_MIDDLE)) {
				// Calculate new camera position based on the mouse movement
				glm::vec2 delta = _graph->_inputManager.calculatePanningDelta(mouseCoordsVec / main_camera2D->getScale());
				/*_graph->_camera.move(delta);*/
				main_camera2D->setPosition_X(_graph->_inputManager.getStartDragPos().x - delta.x);
				main_camera2D->setPosition_Y(_graph->_inputManager.getStartDragPos().y - delta.y);
			}

		case SDL_MOUSEBUTTONDOWN:
			if (_graph->_inputManager.isKeyPressed(SDL_BUTTON_LEFT)) {
				glm::vec2 mouseCoordsVec = _graph->_inputManager.getMouseCoords();
				std::cout << "clicked at: " << mouseCoordsVec.x << " - " << mouseCoordsVec.y << std::endl;
				if (_selectedEntity == nullptr) {
					selectEntityAtPosition(convertScreenToWorld(mouseCoordsVec));
					std::cout << "convertedScreenToWorld: " << convertScreenToWorld(mouseCoordsVec).x << " - " << convertScreenToWorld(mouseCoordsVec).y << std::endl;
				}
			}
			if (!_graph->_inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
				_selectedEntity = nullptr;
			}

			if (_graph->_inputManager.isKeyPressed(SDL_BUTTON_MIDDLE)) {
				glm::vec2 mouseCoordsVec = _graph->_inputManager.getMouseCoords();
				_graph->_inputManager.setPanningPoint(mouseCoordsVec / main_camera2D->getScale());
				_graph->_inputManager.setStartDragPos(main_camera2D->getPosition());
			}
		}
		if (_graph->_inputManager.isKeyPressed(SDLK_p)) {
			onPauseGraph();
		}

		

	}
}

void Graph::updateUI() {

	// Default ImGui window
	ImGui::Begin("Default UI");
	ImGui::Text("This is a permanent UI element.");
	ImGui::End();

	ImGui::Begin("Background UI");
	ImGui::Text("This is a Background UI element.");
	ImGui::ColorEdit4("Background Color", backgroundColor);
	// Change color based on the debug mode state
	if (_renderDebug) {
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));  // Green for ON
	}
	else {
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.2f, 0.2f, 1.0f));  // Red for OFF
	}

	// Button toggles the debug mode
	if (ImGui::Button("Enable Debug Mode")) {
		_renderDebug = !_renderDebug;  // Toggle the state
	}

	ImGui::PopStyleColor(1);
	ImGui::Text("Camera Position");

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	ImGui::Text("Rect: {x: %f, y: %f, w: %f, h: %f}", main_camera2D->getCameraRect().x, main_camera2D->getCameraRect().y, main_camera2D->getCameraRect().w, main_camera2D->getCameraRect().h);
	
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

	if (_selectedEntity) {
		ImGui::Text("Selected Entity Details");

		// Example: Display components of the selected entity
		if (_selectedEntity->hasComponent<TransformComponent>()) {
			TransformComponent* tr = &_selectedEntity->GetComponent<TransformComponent>();
			ImGui::Text("Position: (%f, %f)", tr->getPosition().x, tr->getPosition().y);
			ImGui::Text("Size: (%d, %d)", tr->width, tr->height);
		}
	}
	ImGui::End();
	ImGui::Begin("Performance");
	if (ImPlot::BeginPlot("FPS Plot")) {
#if defined(_WIN32) || defined(_WIN64)
		int plot_count = min(getInterfaceGraph()->getFPSLimiter().fps_history_count,
			getInterfaceGraph()->getFPSLimiter().fpsHistoryIndx); // Ensuring we do not read out of bounds
		int plot_offset = max(0,
			getInterfaceGraph()->getFPSLimiter().fpsHistoryIndx - getInterfaceGraph()->getFPSLimiter().fps_history_count); // Ensure a positive offset
#else
		int plot_count = std::min(getInterfaceGraph()->getFPSLimiter().fps_history_count,
			getInterfaceGraph()->getFPSLimiter().fpsHistoryIndx); // Ensuring we do not read out of bounds
		int plot_offset = std::max(0,
			getInterfaceGraph()->getFPSLimiter().fpsHistoryIndx - getInterfaceGraph()->getFPSLimiter().fps_history_count); // Ensure a positive offset

#endif
		
		ImPlot::SetupAxesLimits(0, 100, 0, 70);

		ImPlot::PlotLine("FPS", &getInterfaceGraph()->getFPSLimiter().fpsHistory[0], plot_count);

		ImPlot::EndPlot();
	}
	ImGui::End();

}

void Graph::renderBatch(const std::vector<Entity*>& entities, SpriteBatch& batch) { 
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	batch.begin();
	for (const auto& entity : entities) {
		if (!entity->getIsHud() && entity->hasComponent<Rectangle_w_Color>()) {
			Rectangle_w_Color entityRectangle = entity->GetComponent<Rectangle_w_Color>();
			if (entity->checkCollision(entityRectangle.destRect, main_camera2D->getCameraRect())) { //draw culling
				entity->draw(batch, *Graph::_window);
			}
		}
		else {
			entity->draw(batch, *Graph::_window);
		}
	}
	batch.end();
	batch.renderBatch();
}

void Graph::draw()
{
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	////////////OPENGL USE
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);


	/////////////////////////////////////////////////////
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glm::mat4 cameraMatrix = main_camera2D->getCameraMatrix();

	// Debug Rendering
	if (_renderDebug) {

		int cellIndex = 0;
		for (const auto& cell : manager.grid->getCells()) {
			// Calculate the position of the cell in world coordinates based on its index
			int x = (cellIndex % manager.grid->getNumXCells()) * manager.grid->getCellSize();
			int y = (cellIndex / manager.grid->getNumXCells()) * manager.grid->getCellSize();

			glm::vec4 destRect(x, y, manager.grid->getCellSize(), manager.grid->getCellSize());
			_debugRenderer.drawBox(destRect, Color(0, 255, 0, 100), 0.0f);  // Drawing each cell in red for visibility

			cellIndex++;
		}
		for (std::size_t group = Manager::groupBackgroundLayer; group != Manager::buttonLabels; group++) {

			std::vector<Entity*>& groupVec = manager.getGroup(group);
			for (auto& entity : groupVec) {
				
				if (entity->hasComponent<TransformComponent>())
				{
					TransformComponent* tr = &entity->GetComponent<TransformComponent>();

					glm::vec4 destRect;
					destRect.x = tr->getPosition().x;
					destRect.y = tr->getPosition().y;
					destRect.z = tr->width;
					destRect.w = tr->height;
					_debugRenderer.drawBox(destRect, Color(255, 255, 255, 255), 0.0f, 0.0f); //todo add angle for drawbox
					//_debugRenderer.drawCircle(glm::vec2(tr->position.x, tr->position.y), Color(255, 255, 255, 255), tr->getCenterTransform().x);
					//break;
				}
				
			}
		}
		if (_selectedEntity) {
			TransformComponent* tr = &_selectedEntity->GetComponent<TransformComponent>();

			glm::vec4 destRect;
			destRect.x = tr->getPosition().x;
			destRect.y = tr->getPosition().y;
			destRect.z = tr->width;
			destRect.w = tr->height;
			_debugRenderer.drawBox(destRect, Color(255, 255, 0, 255), 0.0f, 0.0f); //todo add angle for drawbox
		}

		
	}
	_debugRenderer.drawLine(nodes[0]->GetComponent<TransformComponent>().getCenterTransform(), nodes[1]->GetComponent<TransformComponent>().getCenterTransform(), Color(255, 0, 0, 255), 0.0f);

	_debugRenderer.end();
	_debugRenderer.render(cameraMatrix, 2.0f);
	_resourceManager.setupShader(*_resourceManager.getGLSLProgram("circleColor"), "", *main_camera2D);
	renderBatch(nodes, _spriteBatch);
	renderBatch(cursors, _spriteBatch);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	drawHUD(labels, "arial");
	_resourceManager.getGLSLProgram("color")->use();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindTexture(GL_TEXTURE_2D, 0);

	///////////////////////////////////////////////////////
	_resourceManager.setupShader(*_resourceManager.getGLSLProgram("color"), "", *main_camera2D);

	_resourceManager.getGLSLProgram("circleColor")->unuse();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void Graph::drawHUD(const std::vector<Entity*>& entities, const std::string& textureName) {
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	_resourceManager.setupShader(*_resourceManager.getGLSLProgram("texture"), textureName, *hud_camera2D);
	renderBatch(entities, _hudSpriteBatch);
}

bool Graph::onPauseGraph() {
	_prevScreenIndex = SCREEN_INDEX_MAIN_MENU;
	_currentState = ScreenState::CHANGE_PREVIOUS;
	return true;
}