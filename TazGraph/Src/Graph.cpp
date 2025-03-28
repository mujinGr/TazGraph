
#include "Graph.h"
#include "TextureManager/TextureManager.h"
#include "Camera2.5D/CameraManager.h"
#include "Map/Map.h"
#include "GECS/Components.h"
#include "GECS/ScriptComponents.h"
#include "Collision/Collision.h"
#include "Map/Map.h"
#include "AssetManager/AssetManager.h"
#include <sstream>
#include "GraphScreen/AppInterface.h"
#include <unordered_set>

#undef main

Manager manager;

Map* Graph::map = nullptr;
TazGraphEngine::Window* Graph::_window = nullptr;

auto& world_map(manager.addEntityNoId<Empty>());

glm::vec3 pointAtZ0;

// Get point on the ray at z = -100
glm::vec3 pointAtO;
float nodeRadius = 1.0f;

Graph::Graph(TazGraphEngine::Window* window)
{
	_window = window;
	_sceneIndex = SCENE_INDEX_GRAPHPLAY;
}
Graph::~Graph()
{

}

int Graph::getNextSceneIndex() const {
	return _nextSceneIndex;
}

int Graph::getPreviousSceneIndex() const {
	return _prevSceneIndex;
}

void Graph::build() {

}

void Graph::destroy() {

}

void Graph::onEntry()
{
	/////////////////////////////////////////////
	_resourceManager.addGLSLProgram("color");
	_resourceManager.addGLSLProgram("circleColor");
	_resourceManager.addGLSLProgram("texture");
	_resourceManager.addGLSLProgram("framebuffer");

	_assetsManager = new AssetManager(&manager, _app->_inputManager, _app->_window);

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	_resourceManager.addGLSLProgram("lineColor");

	main_camera2D->init(); // Assuming a screen resolution of 800x600
	main_camera2D->setPosition_X(main_camera2D->getPosition().x /*+ glm::vec2(
		width / 2.0f,
		height / 2.0f
	)*/);
	main_camera2D->setPosition_Y(main_camera2D->getPosition().y);
	main_camera2D->setScale(1.0f);

	hud_camera2D->init();

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

		_resourceManager.getGLSLProgram("framebuffer")->compileShaders("Src/Shaders/framebuffer.vert", "Src/Shaders/framebuffer.frag");
		_resourceManager.getGLSLProgram("framebuffer")->addAttribute("inPos");
		_resourceManager.getGLSLProgram("framebuffer")->addAttribute("inTexCoords");
		_resourceManager.getGLSLProgram("framebuffer")->linkShaders();

		Graph::_PlaneModelRenderer.init();
		Graph::_hudPlaneModelRenderer.init();

		_resourceManager.getGLSLProgram("lineColor")->compileShadersFromSource(_LineRenderer.VERT_SRC, _LineRenderer.FRAG_SRC);
		_resourceManager.getGLSLProgram("lineColor")->addAttribute("vertexPosition");
		_resourceManager.getGLSLProgram("lineColor")->addAttribute("vertexColor");
		_resourceManager.getGLSLProgram("lineColor")->linkShaders();

		Graph::_LineRenderer.init();
		Graph::_PlaneColorRenderer.init();

	}

	if (TTF_Init() == -1)
	{
		std::cout << "Error : SDL_TTF" << std::endl;
	}

	//add the textures to our texture library
	TextureManager::getInstance().Add_GLTexture("arial", "assets/Fonts/arial_cropped_white.png");
	TextureManager::getInstance().Add_GLTexture("worldMap", "assets/Sprites/worldMap.png");

	Graph::map = new Map(1, 32);

	//main_camera2D->worldDimensions= grid->GetLayerDimensions();
	if (!manager.grid)
	{
		manager.grid = std::make_unique<Grid>(ROW_CELL_SIZE, COLUMN_CELL_SIZE, DEPTH_CELL_SIZE, CELL_SIZE);
		//manager.setThreader(threadPool);
		_assetsManager->CreateWorldMap(world_map);
		manager.setComponentNames();
	}
	
	manager.resetEntityId();

	if (!DataManager::getInstance().mapToLoad.empty()) {
		if (strstr(DataManager::getInstance().mapToLoad.c_str(), ".py") != nullptr) {
			map->loadPythonMap(DataManager::getInstance().mapToLoad.c_str()); // Assuming loadPythonMap is a method for loading Python maps
		}
		else {
			map->loadTextMap(DataManager::getInstance().mapToLoad.c_str());
		}
	}


	Music music = getApp()->getAudioEngine().loadMusic("Sounds/JPEGSnow.ogg");
	music.play(-1);


	_resourceManager.getGLSLProgram("framebuffer")->use();


	glUniform1i(glGetUniformLocation(_resourceManager.getGLSLProgram("framebuffer")->getProgramID(), "screenTexture"), 0);
	_resourceManager.getGLSLProgram("framebuffer")->unuse();

	_framebuffer.init(_app->_window.getScreenWidth(), _app->_window.getScreenHeight());

	//ImGuiIO& io = ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void Graph::onExit() {
	Graph::_PlaneModelRenderer.dispose();
	Graph::_hudPlaneModelRenderer.dispose();
	Graph::_LineRenderer.dispose();
	Graph::_PlaneColorRenderer.dispose();

	_resourceManager.disposeGLSLPrograms();

	//for (Thread& thread : threadPool.threads) {
	//	thread.stop();
	//}
}

auto& nodes(manager.getGroup<NodeEntity>(Manager::groupNodes_0));
auto& group_nodes(manager.getGroup<NodeEntity>(Manager::groupGroupNodes_0));

auto& links(manager.getGroup<LinkEntity>(Manager::groupLinks_0));
auto& group_links(manager.getGroup<LinkEntity>(Manager::groupGroupLinks_0));

auto& backgroundImage(manager.getGroup<EmptyEntity>(Manager::panelBackground));

glm::vec2 convertScreenToWorld(glm::vec2 screenCoords) {
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	return main_camera2D->convertScreenToWorld(screenCoords);
}

void Graph::update(float deltaTime) //game objects updating
{
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	glm::vec2 mouseCoordsVec = _sceneMousePosition;

	

	manager.refresh(main_camera2D.get());

	/*glm::vec3 cameraAimPos = main_camera2D->getAimPos();

	glm::vec3 directionToCamera = glm::normalize(cameraAimPos - main_camera2D->eyePos);

	glm::vec3 cameraEulerAngles = main_camera2D->getEulerAnglesFromDirection(directionToCamera);
		*/
	main_camera2D->update();
	hud_camera2D->update();
	if (_firstLoop) {
		manager.updateFully(deltaTime);
		_firstLoop = false;
	}
	else {
		std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

		manager.update(deltaTime);
	}

	// make camera not being able to move out of bounds
	//collision.moveFromOuterBounds();


	/*if (main_camera2D->getScale() < gridLevels[manager.grid->getGridLevel() + 1] ) {
		manager.grid->setGridLevel(static_cast<GridLevel>(manager.grid->getGridLevel() + 1));


	}
	else if (main_camera2D->getScale() > gridLevels[manager.grid->getGridLevel() - 1] ) {
		manager.grid->setGridLevel(static_cast<GridLevel>(manager.grid->getGridLevel() - 1));


	}*/

	if (main_camera2D->getScale() < manager.grid->getLevelScale(manager.grid->getGridLevel())) {
		manager.grid->setGridLevel(static_cast<Grid::Level>(manager.grid->getGridLevel() + 1));
		
		if (manager.grid->getGridLevel() == Grid::Level::Outer1) {
			_assetsManager->createGroupLayout(Grid::Level::Outer1);
		}
		else if (manager.grid->getGridLevel() == Grid::Level::Outer2) {
			_assetsManager->createGroupLayout(Grid::Level::Outer2);
		}		
	}
	else if(manager.grid->getGridLevel() && main_camera2D->getScale() > manager.grid->getLevelScale(static_cast<Grid::Level>(manager.grid->getGridLevel() - 1))){


		if (manager.grid->getGridLevel() == Grid::Level::Outer1) {
			_assetsManager->ungroupLayout(Grid::Level::Outer1);
		}
		else if (manager.grid->getGridLevel() == Grid::Level::Outer2) {
			_assetsManager->ungroupLayout(Grid::Level::Outer2);
		}

		manager.grid->setGridLevel(static_cast<Grid::Level>(manager.grid->getGridLevel() - 1));

		
	}

	// check input manager if left mouse is clicked, if yes and the mouse is not on the widget then nullify displayedEntity
	if (_app->_inputManager.isKeyPressed(SDL_BUTTON_LEFT))
	{
		if (!_editorImgui.isMouseOnWidget("Node Display") && !_editorImgui.isMouseOnWidget("Link Display") && !_editorImgui.isMouseOnWidget("Scene Manager")) {
			_displayedEntity = nullptr;
			_sceneManagerActive = false;
		}
	}

}


std::vector<Cell*> Graph::traversedCellsFromRay(
	glm::vec3 rayOrigin,
	glm::vec3 rayDirection,
	float maxDistance
) {

	std::vector<Cell*> hitCells;
	std::unordered_set<Cell*> visitedCells;

	float stepSize = manager.grid->getCellSize() * 0.5f; // Step size for ray traversal
	glm::vec3 step = glm::normalize(rayDirection) * stepSize; // Step vector
	glm::vec3 currentPos = rayOrigin;

	float traveledDistance = 0.0f;

	while (traveledDistance < maxDistance) {
		int x = static_cast<int>(floor(currentPos.x / manager.grid->getCellSize()));
		int y = static_cast<int>(floor(currentPos.y / manager.grid->getCellSize()));
		int z = static_cast<int>(floor(currentPos.z / manager.grid->getCellSize()));

		// Check if the cell is within the grid bounds
		if (
			x < ceil(manager.grid->getNumXCells() / 2.0f) && x >= ceil(-manager.grid->getNumXCells() / 2.0f) &&
			y < ceil(manager.grid->getNumYCells() / 2.0f) && y >= ceil(-manager.grid->getNumYCells() / 2.0f) &&
			z < ceil(manager.grid->getNumZCells() / 2.0f) && z >= ceil(-manager.grid->getNumZCells() / 2.0f)
			) {
			Cell* cell = manager.grid->getCell(x, y, z, manager.grid->getGridLevel());

			if (cell && visitedCells.find(cell) == visitedCells.end()) {
				hitCells.push_back(cell);
				visitedCells.insert(cell);

				// Add adjacent cells
				for (Cell* adjCell : manager.grid->getAdjacentCells(x, y, z, manager.grid->getGridLevel())) {
					if (adjCell && visitedCells.find(adjCell) == visitedCells.end()) {
						hitCells.push_back(adjCell);
						visitedCells.insert(adjCell);
					}
				}
			}
		}

		// Move along the ray
		currentPos += step;
		traveledDistance += stepSize;
	}

	return hitCells;
}

void Graph::selectEntityFromRay(glm::vec3 rayOrigin, glm::vec3 rayDirection, int activateMode) {
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	std::vector<Cell*> trav_cells = traversedCellsFromRay(rayOrigin, rayDirection, 10000.0f);
	
	bool hasSelected = false;

	for (auto& trav_cell : trav_cells) {
		for (auto& node : trav_cell->nodes) {
			glm::vec3 t;
			TransformComponent* tempBod = &node->GetComponent<TransformComponent>();
			if (rayIntersectsBox(rayOrigin,
				rayDirection,
				glm::vec3(tempBod->position.x, tempBod->position.y, node->GetComponent<TransformComponent>().getPosition().z),
				glm::vec3(tempBod->position.x + tempBod->size.x, tempBod->position.y + tempBod->size.y, node->GetComponent<TransformComponent>().getPosition().z + tempBod->size.z),
				t)) {
				std::cout << "Ray hit node: " << node->getId() << " at distance " << t.x << t.y << t.z << std::endl;
				if (activateMode == SDL_BUTTON_RIGHT)
				{
					_displayedEntity = node;
				}
				else if (activateMode == SDL_BUTTON_LEFT) {
					auto it = std::find_if(_selectedEntities.begin(), _selectedEntities.end(),
						[node](const std::pair<Entity*, glm::vec3>& entry) {
							return entry.first == node;
						});

					if (it == _selectedEntities.end()) { // Node not found
						_selectedEntities.clear();
						_selectedEntities.emplace_back(node, node->GetComponent<TransformComponent>().getPosition() - t);
					}
					else {
						std::vector<std::pair<Entity*, glm::vec3>> updatedSelection;
						updatedSelection.reserve(_selectedEntities.size());

						for (const auto& [entity, _] : _selectedEntities) {
							Node* nodeEntity = dynamic_cast<Node*>(entity);
							if (nodeEntity) {
								glm::vec3 relativePos = nodeEntity->GetComponent<TransformComponent>().getPosition() - t;
								updatedSelection.emplace_back(entity, relativePos);
							}
							else {
								updatedSelection.emplace_back(entity, glm::vec3(0));
							}
						}

						_selectedEntities = std::move(updatedSelection);
					}
				}
				else if (activateMode == ON_HOVER && _selectedEntities.empty()) {
					_onHoverEntity = node;
				}
				else if (activateMode == CTRLD_LEFT_CLICK) {

					auto it = std::find_if(_selectedEntities.begin(), _selectedEntities.end(),
						[node](const std::pair<Entity*, glm::vec3>& entry) {
							return entry.first == node;
						});


					//! update selectedEntities relative positions to center
					std::vector<std::pair<Entity*, glm::vec3>> updatedSelection;
					updatedSelection.reserve(_selectedEntities.size());

					for (const auto& [entity, _] : _selectedEntities) {
						Node* nodeEntity = dynamic_cast<Node*>(entity);
						if (nodeEntity) {
							glm::vec3 relativePos = nodeEntity->GetComponent<TransformComponent>().getCenterTransform() - t;
							updatedSelection.emplace_back(entity, relativePos);
						}
						else {
							updatedSelection.emplace_back(entity, glm::vec3(0));
						}
					}

					if (it == _selectedEntities.end()) {
						glm::vec3 newNodeRelativePos = node->GetComponent<TransformComponent>().getCenterTransform() - t;
						updatedSelection.emplace_back(node, newNodeRelativePos);
					}

					_selectedEntities = std::move(updatedSelection);
				}

				hasSelected = true;
				break;
			}
		}

		if (hasSelected) return;
	}
	

	for (auto& trav_cell : trav_cells) {

		for (auto& link : trav_cell->links) {
			glm::vec3 t;

			if (rayIntersectsLineSegment(rayOrigin,
				rayDirection,
				link->getFromNode()->GetComponent<TransformComponent>().getCenterTransform(),
				link->getToNode()->GetComponent<TransformComponent>().getCenterTransform(),
				t)) {
				std::cout << "Ray hit link: " << link->getId() << " at distance " << t.x << t.y << t.z << std::endl;

				if (activateMode == SDL_BUTTON_RIGHT)
				{
					_displayedEntity = link;
				}
				else if (activateMode == SDL_BUTTON_LEFT) {
					auto it = std::find_if(_selectedEntities.begin(), _selectedEntities.end(),
						[link](const std::pair<Entity*, glm::vec3>& entry) {
							return entry.first == link;
						});

					if (it == _selectedEntities.end()) { // Node not found
						_selectedEntities.clear();
						_selectedEntities.emplace_back(link, glm::vec3(0));
					}
				}
				else if (activateMode == ON_HOVER && _selectedEntities.empty()) {
					_onHoverEntity = link;
				}
				else if (activateMode == CTRLD_LEFT_CLICK) {
					auto it = std::find_if(_selectedEntities.begin(), _selectedEntities.end(),
						[link](const std::pair<Entity*, glm::vec3>& entry) {
							return entry.first == link;
						});

					if (it == _selectedEntities.end()) { // Node not found
						_selectedEntities.emplace_back(link, glm::vec3(0));
					}
					
				}

				hasSelected = true;
				break;
			}
		}

		if (hasSelected) return;

		
	}

	if (!hasSelected && activateMode == SDL_BUTTON_LEFT) {
		_selectedEntities.clear();
	}
}

void Graph::checkInput() {
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt)) {
		ImGui_ImplSDL2_ProcessEvent(&evnt);
		_app->onSDLEvent(evnt);

		glm::vec2 mouseCoordsVec = _sceneMousePosition; // in graph we have another variable for the worldCoords of mouse
		
		glm::vec3 rayOrigin = main_camera2D->getPosition(); // Camera position
		glm::vec3 rayDirection = main_camera2D->castRayAt(mouseCoordsVec); // Ray direction
		
		switch (evnt.type)
		{
		case SDL_MOUSEWHEEL:
			if (!_editorImgui.isMouseInSecondColumn || _displayedEntity) {
				return;
			}
			if (evnt.wheel.y > 0)
			{
				// Scrolling up
				main_camera2D->movePosition_Forward(20.0f);
			}
			else if (evnt.wheel.y < 0)
			{
				// Scrolling down
				main_camera2D->movePosition_Forward(-20.0f);
			}
			break;
		case SDL_KEYDOWN:
			if (_displayedEntity) {
				return;
			}
			if (_app->_inputManager.isKeyDown(SDLK_e)) {
				main_camera2D->movePosition_Forward(10.0f);
			}
			if (_app->_inputManager.isKeyDown(SDLK_r)) {
				main_camera2D->movePosition_Forward(-10.0f);
			}
			if (_app->_inputManager.isKeyDown(SDLK_w)) {
				main_camera2D->movePosition_Vert(10.0f);
			}
			if (_app->_inputManager.isKeyDown(SDLK_s)) {
				main_camera2D->movePosition_Vert(-10.0f);
			}
			if (_app->_inputManager.isKeyDown(SDLK_a)) {
				main_camera2D->movePosition_Hor(-10.0f);
			}
			if (_app->_inputManager.isKeyDown(SDLK_d)) {
				main_camera2D->movePosition_Hor(10.0f);
			}
		case SDL_MOUSEMOTION:
		{
			ImVec2 mainViewportSize = ImGui::GetMainViewport()->Size;

			glm::vec2 ogMouseCoordsVec = _app->_inputManager.getMouseCoords();

			glm::vec2 viewportSize(mainViewportSize.x , mainViewportSize.y);

			glm::vec2 windowPos(_windowPos.x, _windowPos.y);
			glm::vec2 windowDimension(_window->getScreenWidth(), _window->getScreenHeight());
			glm::vec2 windowSize(_windowSize.x, _windowSize.y);

			glm::vec2 getCameraMousePos = _app->_inputManager.convertWindowToCameraCoords(
				ogMouseCoordsVec,
				viewportSize,
				windowDimension,
				windowPos, windowSize,
				*main_camera2D
			);

			_sceneMousePosition = getCameraMousePos;

			bool wasHoveringEntity = _onHoverEntity ? true : false;

			_onHoverEntity = nullptr;

			selectEntityFromRay(rayOrigin, rayDirection, ON_HOVER);

			std::unordered_set<Entity*> connectedEntities;

			if (wasHoveringEntity && !_onHoverEntity) {
				for (NodeEntity* node_entity : manager.getGroup<NodeEntity>(Manager::groupNodes_0)) {
					if (node_entity->hasComponent<Rectangle_w_Color>()) {
						int alpha = 255;
						node_entity->GetComponent<Rectangle_w_Color>().color.a = alpha;
					}
				}
				for (LinkEntity* link_entity : manager.getVisibleGroup<LinkEntity>(Manager::groupLinks_0)) {
					if (link_entity->hasComponent<Line_w_Color>()) {
						int alpha = 255;
						link_entity->GetComponent<Line_w_Color>().src_color.a = alpha;
						link_entity->GetComponent<Line_w_Color>().dest_color.a = alpha;
					}
				}
			}

			if (_onHoverEntity) {
				// todo here reduce the alpha of all nodes and links except the ones that are connected to the the node or the nodes connecting to the link
				Node* hoveredNode = dynamic_cast<Node*>(_onHoverEntity);
				Link* hoveredLink = dynamic_cast<Link*>(_onHoverEntity);
				if (hoveredNode) {
					connectedEntities.insert(hoveredNode);
					for (LinkEntity* link : hoveredNode->getInLinks()) {
						connectedEntities.insert(link);
						connectedEntities.insert(link->getFromNode());
					}
					for (LinkEntity* link : hoveredNode->getOutLinks()) {
						connectedEntities.insert(link);
						connectedEntities.insert(link->getToNode());
					}
				}
				else if (hoveredLink) {
					connectedEntities.insert(hoveredLink);
					connectedEntities.insert(hoveredLink->getFromNode());
					connectedEntities.insert(hoveredLink->getToNode());
				}

				for (NodeEntity* node_entity : manager.getVisibleGroup<NodeEntity>(Manager::groupNodes_0)) {
					if (node_entity->hasComponent<Rectangle_w_Color>()) {
						int alpha = (connectedEntities.empty() || connectedEntities.count(node_entity)) ? 255 : 100;
						node_entity->GetComponent<Rectangle_w_Color>().color.a = alpha;
					}
				}
				for (LinkEntity* link_entity : manager.getVisibleGroup<LinkEntity>(Manager::groupLinks_0)) {
					if (link_entity->hasComponent<Line_w_Color>()) {
						int alpha = (connectedEntities.empty() || connectedEntities.count(link_entity)) ? 255 : 100;
						link_entity->GetComponent<Line_w_Color>().src_color.a = alpha;
						link_entity->GetComponent<Line_w_Color>().dest_color.a = alpha;
					}
				}
			}

			if (_app->_inputManager.isKeyDown(SDL_BUTTON_LEFT) && !_selectedEntities.empty()) {
				

				glm::vec3 pointAtCenterAxis = glm::vec3(0.0f);
				
				glm::vec3 center(0.0f);
				int nodeEntitiesSize = 0;

				for (const auto& [entity, _] : _selectedEntities) {
					Node* nodeEntity = dynamic_cast<Node*>(entity);
					if (nodeEntity) {
						center += nodeEntity->GetComponent<TransformComponent>().getCenterTransform();
						nodeEntitiesSize++;
					}
				}

				center /= nodeEntitiesSize;  // Include new node

				pointAtCenterAxis = main_camera2D->getPointOnRayAtZ(rayOrigin, rayDirection, center.z);
						
				for (const auto& [entity, relativePos] : _selectedEntities) {
					Node* nodeEntity = dynamic_cast<Node*>(entity);
					if (nodeEntity) {
						entity->GetComponent<TransformComponent>().setPosition_X(pointAtCenterAxis.x + relativePos.x);
						entity->GetComponent<TransformComponent>().setPosition_Y(pointAtCenterAxis.y + relativePos.y);
					}
				}
			}

			if (_app->_inputManager.isKeyDown(SDL_BUTTON_MIDDLE)) {
				// Calculate new camera position based on the mouse movement
				glm::vec3 delta = glm::vec3(_app->_inputManager.calculatePanningDelta(mouseCoordsVec ),0.0f);
				main_camera2D->moveAimPos(_app->_inputManager.getStartDragAimPos(), delta);
			}
		}
		case SDL_MOUSEBUTTONDOWN:
			if (!_editorImgui.isMouseInSecondColumn) {
				return;
			}
			if ((_app->_inputManager.isKeyDown(SDLK_RCTRL) || _app->_inputManager.isKeyDown(SDLK_LCTRL)) &&
				_app->_inputManager.isKeyPressed(SDL_BUTTON_LEFT) &&
				!_selectedEntities.empty()
				) {
				selectEntityFromRay(rayOrigin, rayDirection, CTRLD_LEFT_CLICK);

				// Get point on the ray at z = 0
				pointAtZ0 = main_camera2D->getPointOnRayAtZ(rayOrigin, rayDirection, 0.0f);

				// Get point on the ray at z = -100
				pointAtO = main_camera2D->getPointOnRayAtZ(rayOrigin, rayDirection, rayOrigin.z);

			}
			else if (_app->_inputManager.isKeyPressed(SDL_BUTTON_LEFT)) { // this is for selection and moving around nodes
				selectEntityFromRay(rayOrigin, rayDirection, SDL_BUTTON_LEFT);
					
				// Get point on the ray at z = 0
				pointAtZ0 = main_camera2D->getPointOnRayAtZ(rayOrigin, rayDirection, 0.0f);

				// Get point on the ray at z = -100
				pointAtO = main_camera2D->getPointOnRayAtZ(rayOrigin, rayDirection, rayOrigin.z);
				
			}

			if (_app->_inputManager.isKeyPressed(SDL_BUTTON_MIDDLE)) {
				_app->_inputManager.setPanningPoint(mouseCoordsVec);
				_app->_inputManager.setStartDragAimPos(main_camera2D->getAimPos());
			}
			if (_app->_inputManager.isKeyPressed(SDL_BUTTON_RIGHT)) {
				std::cout << "right-clicked at: " << mouseCoordsVec.x << " - " << mouseCoordsVec.y << std::endl;

				selectEntityFromRay(rayOrigin, rayDirection, SDL_BUTTON_RIGHT);

				if (!_displayedEntity && _editorImgui.isMouseInSecondColumn) {
					_sceneManagerActive = true;
				}

				_savedMainViewportMousePosition = _app->_inputManager.getMouseCoords();
			}
		case SDL_MOUSEBUTTONUP:
			if (!_app->_inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
				//_selectedEntities = nullptr;
			}
		}
		
		if (_app->_inputManager.isKeyPressed(SDLK_p)) {
			onPauseGraph();
		}

		

	}
}

void Graph::BeginRender() {
	_editorImgui.BeginRender();
}

void Graph::updateUI() {
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::Begin("Main Window", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);
	_editorImgui.MenuBar();

	ImGui::Columns(3, "mycolumns");

	static bool initializedUIColumns = false; // Flag to ensure widths are set only once
	if (!initializedUIColumns) {
		float totalWidth = ImGui::GetContentRegionAvail().x;
		ImGuiIO& io = ImGui::GetIO();
		ImVec2 whole_content_size = io.DisplaySize;

		ImGui::SetColumnWidth(0, whole_content_size.x * 0.2f);
		ImGui::SetColumnWidth(1, whole_content_size.x * 0.6f);
		ImGui::SetColumnWidth(2, whole_content_size.x * 0.2f);

		initializedUIColumns = true; // Prevents reapplying widths
	}

	ImGui::BeginChild("Tab 1");

	_editorImgui.BackGroundUIElement(_renderDebug, _sceneMousePosition, _app->_inputManager.getMouseCoords(), manager, _onHoverEntity, _backgroundColor, CELL_SIZE);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 350.0f);
	_editorImgui.FPSCounter(getApp()->getFPSLimiter());
	
	ImGui::EndChild();

	ImGui::NextColumn();
	
	_editorImgui.SceneTabs();

	_editorImgui.updateIsMouseInSecondColumn();

	_editorImgui.SceneViewport(_framebuffer._framebufferTexture, _windowPos, _windowSize);
	ImGui::NextColumn();
	ImGui::BeginChild("Tab 2");

	_editorImgui.ShowAllEntities(manager, nodeRadius);
	_editorImgui.availableFunctions();
	_editorImgui.ShowFunctionExecutionResults();
	ImGui::EndChild();

	ImGui::End();

	if (_editorImgui.isSaving()) {
		_editorImgui.SavingUI(map);
	}
	if (_editorImgui.isLoading()) {
		char* loadMapPath = _editorImgui.LoadingUI();
		if (!_editorImgui.isLoading()) {
			if (strstr(loadMapPath, ".py") != nullptr) {
				map->loadPythonMap(loadMapPath); // Assuming loadPythonMap is a method for loading Python maps
			}
			else {
				map->loadTextMap(loadMapPath);
			}
		}
	}
	if (_editorImgui.isGoingBack()) {
		_currentState = SceneState::CHANGE_PREVIOUS;
		_editorImgui.SetGoingBack(false);
	}

	
	//glm::vec2 worldToVieport
	_editorImgui.ShowStatisticsAbout(_savedMainViewportMousePosition, _displayedEntity, manager);

	if (_sceneManagerActive) {
		_editorImgui.ShowSceneControl(_savedMainViewportMousePosition, manager);
	}
	// this is going to be shown when right click on scene and no displayEntity shows


	glClearColor(_backgroundColor[0], _backgroundColor[1], _backgroundColor[2], _backgroundColor[3]);
}

void Graph::EndRender() {
	_editorImgui.EndRender();
}

void Graph::renderBatch(const std::vector<LinkEntity*>& entities, LineRenderer& batch) {
		for (int i = 0; i < entities.size(); i++) {
			if (entities[i]->hasComponent<Line_w_Color>()) {
				entities[i]->GetComponent<Line_w_Color>().draw(i, batch, *Graph::_window);
			}
		}

		//! activate threads near the end, where we have completed everything else
	//threadPool.parallel(entities.size(), [&](int start, int end) {
	//	for (int i = start; i < end; i++) {
	//		if (entities[i]->hasComponent<Line_w_Color>()) {
	//			entities[i]->GetComponent<Line_w_Color>().draw(i + startIndex, batch, *Graph::_window);
	//		}
	//	}
	//	});
}

void Graph::renderBatch(const std::vector<NodeEntity*>& entities, PlaneColorRenderer& batch) {

	//threadPool.parallel(entities.size(), [&](int start, int end) {
	//	for (int i = start; i < end; i++) {
	//		if (entities[i]->hasComponent<Rectangle_w_Color>()) {

	//			entities[i]->GetComponent<Rectangle_w_Color>().draw(i, batch, *Graph::_window);
	//		}
	//	}
	//});

		for (int i = 0; i < entities.size(); i++) {
			if (entities[i]->hasComponent<Rectangle_w_Color>()) {

				entities[i]->GetComponent<Rectangle_w_Color>().draw(i, batch, *Graph::_window);
			}
		}
}

void Graph::renderBatch(const std::vector<EmptyEntity*>& entities, PlaneColorRenderer& batch) {

	/*threadPool.parallel(entities.size(), [&](int start, int end) {
		for (int i = start; i < end; i++) {
			if (entities[i]->hasComponent<Rectangle_w_Color>()) {
				entities[i]->GetComponent<Rectangle_w_Color>().draw(i, batch, *Graph::_window);
			}
			if (entities[i]->hasComponent<Triangle_w_Color>()) {
				entities[i]->GetComponent<Triangle_w_Color>().draw(i, batch, *Graph::_window);
			}
		}
		});*/

		for (int i = 0; i < entities.size(); i++) {
			if (entities[i]->hasComponent<Rectangle_w_Color>()) {
				entities[i]->GetComponent<Rectangle_w_Color>().draw(i, batch, *Graph::_window);
			}
			if (entities[i]->hasComponent<Triangle_w_Color>()) {
				entities[i]->GetComponent<Triangle_w_Color>().draw(i, batch, *Graph::_window);
			}
		}
}

void Graph::renderBatch(const std::vector<NodeEntity*>& entities, PlaneModelRenderer& batch) { 
	// before calling this make sure that reserved the right amount of memory
	//for (const auto& entity : entities) {
	//	entity->draw(batch, *Graph::_window);
	//}

	for (int i = 0; i < entities.size(); i++) {
		entities[i]->draw(i, batch, *Graph::_window);
	}

}
void Graph::renderBatch(const std::vector<EmptyEntity*>& entities, PlaneModelRenderer& batch) {
	// before calling this make sure that reserved the right amount of memory
	/*for (const auto& entity : entities) {
		entity->draw(batch, *Graph::_window);
	}*/

	for (int i = 0; i < entities.size(); i++) {
		entities[i]->draw(i, batch, *Graph::_window);
	}
}

void Graph::draw()
{
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	GLSLProgram glsl_texture		= *_resourceManager.getGLSLProgram("texture");
	GLSLProgram glsl_lineColor		= *_resourceManager.getGLSLProgram("lineColor");
	GLSLProgram glsl_circleColor	= *_resourceManager.getGLSLProgram("circleColor");
	GLSLProgram glsl_color			= *_resourceManager.getGLSLProgram("color");
	GLSLProgram glsl_framebuffer	= *_resourceManager.getGLSLProgram("framebuffer");

	_framebuffer.Bind();
	////////////OPENGL USE
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);



	/////////////////////////////////////////////////////
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glm::mat4 rotationMatrix = glm::mat4(1.0f);
	glm::vec3 cameraAimPos = main_camera2D->getAimPos();
	glm::vec3 directionToCamera = glm::normalize(cameraAimPos - main_camera2D->eyePos);
	glm::vec3 cameraEulerAngles = main_camera2D->getEulerAnglesFromDirection(directionToCamera);

	rotationMatrix = getRotationMatrix(cameraEulerAngles);

	//_PlaneModelRenderer.begin();

	/*_resourceManager.setupShader(*_resourceManager.getGLSLProgram("texture"), "worldMap", *main_camera2D);
	renderBatch(backgroundImage, _PlaneModelRenderer, false);
	_PlaneModelRenderer.end();
	_PlaneModelRenderer.renderBatch();*/

	// Debug Rendering
	if (_renderDebug) {
		_LineRenderer.begin();
		_resourceManager.setupShader(glsl_lineColor, *main_camera2D);

		std::vector<Cell*> intercectedCells = manager.grid->getIntersectedCameraCells(*main_camera2D);
		
		_LineRenderer.initBatchSquares(4);
		
		_LineRenderer.initBatchBoxes(
			intercectedCells.size() +
			manager.getVisibleGroup<NodeEntity>(Manager::groupNodes_0).size() +
			manager.getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_0).size() +
			manager.getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_1).size() 
		);
		

		_LineRenderer.initBatchSize();

		size_t v_index = 0;

		_LineRenderer.drawRectangle(v_index++, glm::vec4(-ROW_CELL_SIZE / 2, -COLUMN_CELL_SIZE / 2, ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2), Color(255, 0, 255, 255), 0.0f, 0.0f);
		_LineRenderer.drawRectangle(v_index++, glm::vec4(0, -COLUMN_CELL_SIZE / 2, ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2), Color(255, 0, 255, 255), 0.0f, 0.0f);
		_LineRenderer.drawRectangle(v_index++, glm::vec4(-ROW_CELL_SIZE / 2, 0, ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2), Color(255, 0, 255, 255), 0.0f, 0.0f);
		_LineRenderer.drawRectangle(v_index++, glm::vec4(0, 0, ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2), Color(255, 0, 255, 255), 0.0f, 0.0f);


		size_t box_v_index = 0;

		for (const auto& cell : intercectedCells) {
			glm::vec3 cellBox_org(cell->boundingBox_origin.x, cell->boundingBox_origin.y, cell->boundingBox_origin.z);
			glm::vec3 cellBox_size(cell->boundingBox_size.x, cell->boundingBox_size.y, cell->boundingBox_size.z);

			_LineRenderer.drawBox(box_v_index++, cellBox_org, cellBox_size, Color(0, 255, 0, 20), 0.0f);  // Drawing each cell in red for visibility
		}

		for (auto& group : {
			manager.getVisibleGroup<NodeEntity>(Manager::groupNodes_0),
			manager.getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_0),
			manager.getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_1) 
			}) {
			
			std::vector<NodeEntity*> groupVec = group;
			
			for (auto& entity : groupVec) {

				if (entity->hasComponent<TransformComponent>())
				{
					TransformComponent* tr = &entity->GetComponent<TransformComponent>();

					glm::vec4 destRect;
					destRect.x = tr->getPosition().x;
					destRect.y = tr->getPosition().y;
					destRect.z = tr->size.x;
					destRect.w = tr->size.y;

					glm::vec3 nodeBox_org(destRect.x, destRect.y, tr->getPosition().z);
					glm::vec3 nodeBox_size(destRect.z, destRect.w, tr->size.z);

					_LineRenderer.drawBox(box_v_index++, nodeBox_org, nodeBox_size, Color(255, 255, 255, 255), 0.0f);  // Drawing each cell in red for visibility

					//_LineRenderer.drawCircle(glm::vec2(tr->position.x, tr->position.y), Color(255, 255, 255, 255), tr->getCenterTransform().x);
					//break;
				}

			}
		}
		
		
		_LineRenderer.end();
		_LineRenderer.renderBatch(main_camera2D->getScale() * 10.0f * (manager.grid->getGridLevel() + 1));
		glsl_lineColor.unuse();

	}

	_LineRenderer.begin();
	_PlaneColorRenderer.begin();
	_PlaneModelRenderer.begin();

	_LineRenderer.initBatchLines(
		manager.getVisibleGroup<LinkEntity>(Manager::groupLinks_0).size() +
		manager.getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_0).size() +
		manager.getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_1).size()
	);
	
	_PlaneColorRenderer.initColorQuadBatch(
		manager.getVisibleGroup<NodeEntity>(Manager::groupNodes_0).size() +
		manager.getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_0).size() +
		manager.getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_1).size()
	);
	_PlaneColorRenderer.initColorTriangleBatch(
		manager.getVisibleGroup<EmptyEntity>(Manager::groupArrowHeads_0).size()
	);

	_PlaneModelRenderer.initTextureQuadBatch(
		manager.getVisibleGroup<NodeEntity>(Manager::groupRenderSprites).size()
	);


	_PlaneColorRenderer.initBatchSize();
	_LineRenderer.initBatchSize();
	_PlaneModelRenderer.initBatchSize();

	renderBatch(manager.getVisibleGroup<LinkEntity>(Manager::groupLinks_0), _LineRenderer);

	//renderBatch(1, manager.getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_0), _LineRenderer);
	//renderBatch(1, manager.getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_1), _LineRenderer);
	

	//_LineRenderer.renderBatch(cameraMatrix, 2.0f);


	
	
	renderBatch(manager.getVisibleGroup<NodeEntity>(Manager::groupNodes_0), _PlaneColorRenderer);
	//renderBatch(manager.getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_1), _PlaneColorRenderer);
	

	renderBatch(manager.getVisibleGroup<EmptyEntity>(Manager::groupArrowHeads_0), _PlaneColorRenderer);


	renderBatch(manager.getVisibleGroup<EmptyEntity>(Manager::groupRenderSprites), _PlaneModelRenderer);

	renderBatch(manager.getVisibleGroup<NodeEntity>(Manager::groupRenderSprites), _PlaneModelRenderer);



	_resourceManager.setupShader(glsl_lineColor, *main_camera2D);
	_LineRenderer.end();
	_LineRenderer.renderBatch(main_camera2D->getScale() * 5.0f);
	glsl_lineColor.unuse();
	
	
	_resourceManager.setupShader(glsl_color, *main_camera2D);
	GLint pLocation = glsl_color.getUniformLocation("rotationMatrix");
	glUniformMatrix4fv(pLocation, 1, GL_FALSE, glm::value_ptr(rotationMatrix));
	_PlaneColorRenderer.end();
	_PlaneColorRenderer.renderBatch(_resourceManager.getGLSLProgram("color"));
	glsl_color.unuse();


	_resourceManager.setupShader(glsl_texture, *main_camera2D);
	_PlaneModelRenderer.end();
	_PlaneModelRenderer.renderBatch();
	glsl_texture.unuse();


	_LineRenderer.begin();

	size_t nodeCount = std::count_if(_selectedEntities.begin(), _selectedEntities.end(),
		[](const std::pair<Entity*, glm::vec3>& entry) {
			return dynamic_cast<NodeEntity*>(entry.first) != nullptr;
		});

	size_t linkCount = std::count_if(_selectedEntities.begin(), _selectedEntities.end(),
		[](const std::pair<Entity*, glm::vec3>& entry) {
			return dynamic_cast<LinkEntity*>(entry.first) != nullptr;
		});

	_LineRenderer.initBatchLines(
		linkCount +
		1
	);
	_LineRenderer.initBatchBoxes(
		nodeCount
	);
	
	_LineRenderer.initBatchSize();

	size_t lineIndex = 0;
	size_t boxIndex = 0;


	if (!_selectedEntities.empty()) {

		for (int i = 0; i < _selectedEntities.size(); i++) {
			Node* node = dynamic_cast<Node*>(_selectedEntities[i].first);
			Link* link = dynamic_cast<Link*>(_selectedEntities[i].first);
			if (node) {
				if (_selectedEntities[i].first->hasComponent<TransformComponent>()) {
					TransformComponent* tr = &_selectedEntities[i].first->GetComponent<TransformComponent>();

					glm::vec4 destRect;
					destRect.x = tr->getPosition().x;
					destRect.y = tr->getPosition().y;
					destRect.z = tr->size.x;
					destRect.w = tr->size.y;

					glm::vec3 nodeBox_org(destRect.x, destRect.y, tr->getPosition().z);
					glm::vec3 nodeBox_size(destRect.z, destRect.w, tr->size.z);

					_LineRenderer.drawBox(boxIndex++, nodeBox_org, nodeBox_size, Color(255, 255, 0, 100), 0.0f); //todo add angle for drawRectangle
				}
			}
			else if (link) {
				if (_selectedEntities[i].first->hasComponent<Line_w_Color>()) {
					Line_w_Color* lWc = &_selectedEntities[i].first->GetComponent<Line_w_Color>();

					glm::vec3 startP = lWc->entity->getFromNode()->GetComponent<TransformComponent>().getCenterTransform();
					glm::vec3 endP = lWc->entity->getToNode()->GetComponent<TransformComponent>().getCenterTransform();

					_LineRenderer.drawLine(lineIndex++, startP, endP, Color(255, 255, 0, 100), Color(255, 255, 0, 100));

				}
			}
		}
	}

	_LineRenderer.drawLine(lineIndex++, pointAtZ0, pointAtO, Color(0, 0, 0, 255), Color(0, 0, 255, 255));

	_resourceManager.setupShader(glsl_lineColor, *main_camera2D);
	_LineRenderer.end();
	_LineRenderer.renderBatch(main_camera2D->getScale() * 5.0f);
	glsl_lineColor.unuse();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	/*drawHUD(labels, "arial");
	_resourceManager.getGLSLProgram("color")->use();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/

	glBindTexture(GL_TEXTURE_2D, 0);
	///////////////////////////////////////////////////////

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	_framebuffer.Unbind();

	glClear(GL_COLOR_BUFFER_BIT);
}


void Graph::drawHUD(const std::vector<NodeEntity*>& entities) {
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	_resourceManager.setupShader(*_resourceManager.getGLSLProgram("texture"), *hud_camera2D);
	renderBatch(entities, _hudPlaneModelRenderer);
}

bool Graph::onPauseGraph() {
	_prevSceneIndex = SCENE_INDEX_MAIN_MENU;
	_currentState = SceneState::CHANGE_PREVIOUS;
	return true;
}