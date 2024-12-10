
#include "Graph.h"
#include "TextureManager/TextureManager.h"
#include "Camera2.5D/CameraManager.h"
#include "Map/Map.h"
#include "ECS/Components.h"
#include "ECS/ScriptComponents.h"
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
float Graph::backgroundColor[4] = {0.84f, 0.91f, 1.f, 1.0f};
TazGraphEngine::Window* Graph::_window = nullptr;

auto& player1(manager.addEntity());
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

		_colorProgram.compileShaders("Src/Shaders/colorShading.vert", "Src/Shaders/colorShading.frag");
		_colorProgram.addAttribute("vertexPosition");
		_colorProgram.addAttribute("vertexColor");
		_colorProgram.addAttribute("vertexUV");
		_colorProgram.linkShaders();

		_circleColorProgram.compileShaders("Src/Shaders/circleColorShading.vert", "Src/Shaders/circleColorShading.frag");
		_circleColorProgram.addAttribute("vertexPosition");
		_circleColorProgram.addAttribute("vertexColor");
		_circleColorProgram.addAttribute("vertexUV");
		_circleColorProgram.linkShaders();

		_textureProgram.compileShaders("Src/Shaders/textureBright.vert", "Src/Shaders/textureBright.frag");
		_textureProgram.addAttribute("vertexPosition");
		_textureProgram.addAttribute("vertexColor");
		_textureProgram.addAttribute("vertexUV");
		_textureProgram.linkShaders();

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

	assets->CreatePlayer(player1);
	manager.grid->addEntity(&player1);

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
auto& players(manager.getGroup(Manager::groupPlayers));
auto& colliders(manager.getGroup(Manager::groupColliders));
auto& labels(manager.getGroup(Manager::groupLabels));
auto& lights(manager.getGroup(Manager::groupLights));
auto& screenshapes(manager.getGroup(Manager::screenShapes));
auto& hpbars(manager.getGroup(Manager::groupHPBars));
auto& generators(manager.getGroup(Manager::groupEnvironmentGenerators));

void Graph::update(float deltaTime) //game objects updating
{
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

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

	for (auto& pl : players) //player rules
	{
		main_camera2D->setPosition_X(pl->GetComponent<TransformComponent>().getPosition().x - main_camera2D->getCameraDimensions().x / 2);
		main_camera2D->setPosition_Y(pl->GetComponent<TransformComponent>().getPosition().y - main_camera2D->getCameraDimensions().y / 2);

		collision.moveFromOuterBounds(*pl, *_window);
	}
}

glm::vec2 convertScreenToWorld(glm::vec2 screenCoords) {
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	screenCoords = main_camera2D->convertScreenToWorld(screenCoords);  
	return screenCoords;
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
				_selectedEntity->GetComponent<TransformComponent>().setPosition_X(convertScreenToWorld(mouseCoordsVec ).x);
				_selectedEntity->GetComponent<TransformComponent>().setPosition_Y (convertScreenToWorld(mouseCoordsVec ).y);
				if(_selectedEntity->hasComponent<GridComponent>())
					_selectedEntity->GetComponent<GridComponent>().updateCollidersGrid();
			}
		}
		if (_graph->_inputManager.isKeyPressed(SDLK_p)) {
			onPauseGraph();
		}

		if (_graph->_inputManager.isKeyPressed(SDLK_n)) {
			Graph::map->setMapCompleted(true);
		}

		if (_graph->_inputManager.isKeyPressed(SDL_BUTTON_LEFT)) {
			glm::vec2 mouseCoordsVec = _graph->_inputManager.getMouseCoords();
			if ( _selectedEntity == nullptr) {
				selectEntityAtPosition(convertScreenToWorld(mouseCoordsVec));
			}	
		}
		if (!_graph->_inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
			_selectedEntity = nullptr;
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

	if (_selectedEntity) {
		ImGui::Text("Selected Entity Details");

		// Example: Display components of the selected entity
		if (_selectedEntity->hasComponent<TransformComponent>()) {
			TransformComponent* tr = &_selectedEntity->GetComponent<TransformComponent>();
			ImGui::Text("Position: (%d, %d)", tr->getPosition().x, tr->getPosition().y);
			ImGui::Text("Size: (%d, %d)", tr->width, tr->height);
		}
	}
	ImGui::End();

}

void Graph::setupShader_Texture(GLSLProgram& shaderProgram, const std::string& textureName) {
	glActiveTexture(GL_TEXTURE0);
	const GLTexture* texture = TextureManager::getInstance().Get_GLTexture(textureName);
	glBindTexture(GL_TEXTURE_2D, texture->id);
	GLint textureLocation = shaderProgram.getUniformLocation("texture_sampler");
	glUniform1i(textureLocation, 0);
}

void Graph::setupShader(GLSLProgram& shaderProgram, const std::string& textureName, ICamera& camera) {
	shaderProgram.use();
	if (!textureName.empty()) {
		setupShader_Texture(shaderProgram, textureName);
	}
	GLint pLocation = shaderProgram.getUniformLocation("projection");
	glm::mat4 cameraMatrix = camera.getCameraMatrix();
	glUniformMatrix4fv(pLocation, 1, GL_FALSE, &(cameraMatrix[0][0]));
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

	
	setupShader(_circleColorProgram,"", *main_camera2D);
	renderBatch(nodes, _spriteBatch);
	//renderBatch(colliders);
	//based on weather change Shader of textures

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//renderBatch(lights);

	drawHUD(labels, "arial");
	_colorProgram.use();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindTexture(GL_TEXTURE_2D, 0);

	///////////////////////////////////////////////////////
	setupShader(_colorProgram, "", *main_camera2D);

	renderBatch(hpbars, _spriteBatch);
	_circleColorProgram.unuse();
	
	// Debug Rendering
	if (_renderDebug) {
		glm::mat4 cameraMatrix = main_camera2D->getCameraMatrix();

		int cellIndex = 0;
		for (const auto& cell : manager.grid->getCells()) {
			// Calculate the position of the cell in world coordinates based on its index
			int x = (cellIndex % manager.grid->getNumXCells()) * manager.grid->getCellSize();
			int y = (cellIndex / manager.grid->getNumXCells()) * manager.grid->getCellSize();

			glm::vec4 destRect(x, y, manager.grid->getCellSize(), manager.grid->getCellSize());
			_debugRenderer.drawBox(destRect, Color(0, 255, 0, 25), 0.0f);  // Drawing each cell in red for visibility

			cellIndex++;
		}
		for (auto& p : players) //player with colliders
		{
			for (auto& c : manager.adjacentEntities(p, Manager::groupColliders))
			{
				//SDL_Rect cCol = c->GetComponent<ColliderComponent>().collider;
				for (auto& ccomp : c->components) { // get all the ColliderComponents

					ColliderComponent* colliderComponentPtr = dynamic_cast<ColliderComponent*>(ccomp.get());

					if (!colliderComponentPtr) {
						continue;
					}
					SDL_Rect cCol = ccomp->getRect();
					glm::vec4 destRect(cCol.x, cCol.y, cCol.w, cCol.h);
					_debugRenderer.drawBox(destRect, Color(255, 0, 0, 255), 0.0f);
					_debugRenderer.end();
					_debugRenderer.render(cameraMatrix, 4.0f);
				}
			}
		}
		for (std::size_t group = Manager::groupBackgroundLayer; group != Manager::groupCircles; group++) {

			if (group == Manager::groupHPBars) {
				continue; 
			}

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
					_debugRenderer.drawBox(destRect, Color(255, 255, 255, 255), 0.0f, -5.0f); //todo add angle for drawbox
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
			_debugRenderer.drawBox(destRect, Color(255, 255, 0, 255), 0.0f, -5.0f); //todo add angle for drawbox
		}
		_debugRenderer.end();
		_debugRenderer.render(cameraMatrix, 2.0f);
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void Graph::drawHUD(const std::vector<Entity*>& entities, const std::string& textureName) {
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	setupShader(_textureProgram, textureName, *hud_camera2D);
	renderBatch(entities, _hudSpriteBatch);
}

bool Graph::onPauseGraph() {
	_prevScreenIndex = SCREEN_INDEX_MAIN_MENU;
	_currentState = ScreenState::CHANGE_PREVIOUS;
	return true;
}