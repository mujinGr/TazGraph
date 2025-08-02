
/**
 * @file Graph.cpp

*/

#include "Graph.h"
#include "TextureManager/TextureManager.h"
#include "Camera2.5D/CameraManager.h"
#include "Map/Map.h"
#include "GECS/Components.h"
#include "GECS/ScriptComponents.h"
#include "Map/Map.h"
#include "AssetManager/AssetManager.h"
#include <sstream>
#include "GraphScreen/AppInterface.h"
#include <unordered_set>

#undef main

TazGraphEngine::Window* Graph::_window = nullptr;


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
	std::string mapName = DataManager::getInstance().mapToLoad;

	/////////////////////////////////////////////
	_resourceManager.addGLSLProgram("color");
	_resourceManager.addGLSLProgram("texture");
	_resourceManager.addGLSLProgram("framebuffer");
	_resourceManager.addGLSLProgram("light");


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

		_resourceManager.getGLSLProgram("color")->compileAndLinkShaders("Src/Shaders/colorShading.vert", "Src/Shaders/colorShading.frag");
		_resourceManager.getGLSLProgram("color")->addAttribute("vertexPosition");
		_resourceManager.getGLSLProgram("color")->addAttribute("vertexColor");
		_resourceManager.getGLSLProgram("color")->addAttribute("vertexUV");

		_resourceManager.getGLSLProgram("texture")->compileAndLinkShaders("Src/Shaders/textureBright.vert", "Src/Shaders/textureBright.frag");
		_resourceManager.getGLSLProgram("texture")->addAttribute("vertexPosition");
		_resourceManager.getGLSLProgram("texture")->addAttribute("vertexColor");
		_resourceManager.getGLSLProgram("texture")->addAttribute("vertexUV");

		_resourceManager.getGLSLProgram("framebuffer")->compileAndLinkShaders("Src/Shaders/framebuffer.vert", "Src/Shaders/framebuffer.frag");
		_resourceManager.getGLSLProgram("framebuffer")->addAttribute("inPos");
		_resourceManager.getGLSLProgram("framebuffer")->addAttribute("inTexCoords");

		_resourceManager.getGLSLProgram("light")->compileAndLinkShaders("Src/Shaders/colorLighting.vert", "Src/Shaders/colorLighting.frag");
		_resourceManager.getGLSLProgram("light")->addAttribute("vertexPosition");
		_resourceManager.getGLSLProgram("light")->addAttribute("vertexColor");
		_resourceManager.getGLSLProgram("light")->addAttribute("vertexUV");

		Graph::_PlaneModelRenderer.init();
		generateSphereMesh(
			Graph::_LightRenderer.sphereVertices,
			Graph::_LightRenderer.sphereIndices);

		Graph::_LightRenderer.init();
		Graph::_hudPlaneModelRenderer.init();

		/*_resourceManager.getGLSLProgram("lineColor")->compileShadersFromSource(_LineRenderer.VERT_SRC, _LineRenderer.FRAG_SRC);
		_resourceManager.getGLSLProgram("lineColor")->addAttribute("vertexPosition");
		_resourceManager.getGLSLProgram("lineColor")->addAttribute("vertexColor");
		_resourceManager.getGLSLProgram("lineColor")->linkShaders();*/

		_resourceManager.getGLSLProgram("lineColor")->compileAndLinkShaders("Src/Shaders/lineColorShading.vert", "Src/Shaders/lineColorShading.gs", "Src/Shaders/lineColorShading.frag");
		_resourceManager.getGLSLProgram("lineColor")->addAttribute("vertexPosition");
		_resourceManager.getGLSLProgram("lineColor")->addAttribute("vertexColor");


		Graph::_LineRenderer.init();
		generateSphereMesh(
			Graph::_PlaneColorRenderer.sphereVertices,
			Graph::_PlaneColorRenderer.sphereIndices);
		Graph::_PlaneColorRenderer.init();
	}

	if (TTF_Init() == -1)
	{
		std::cout << "Error : SDL_TTF" << std::endl;
	}

	//add the textures to our texture library
	TextureManager::getInstance().Add_GLTexture("arial", "assets/Fonts/arial_cropped_white.png");
	TextureManager::getInstance().Add_GLTexture("worldMap", "assets/Sprites/worldMap.png");
	TextureManager::getInstance().Add_GLTexture("play-button", "assets/Sprites/images-removebg-preview.png");
	TextureManager::getInstance().Add_GLTexture("treemap", "assets/Sprites/treemap.png");
	TextureManager::getInstance().Add_GLTexture("sauronEye", "assets/Sprites/Eye-of-Sauron.png");

	if (setManager(mapName)) {
		auto& world_map(manager->addEntityNoId<Empty>());
		_assetsManager->CreateWorldMap(world_map);

		manager->resetEntityId();

		map->loadMap(
			DataManager::getInstance().mapToLoad.c_str(),
			std::bind(&Map::AddDefaultNode, map, std::placeholders::_1, std::placeholders::_2),
			std::bind(&Map::AddDefaultLink, map, std::placeholders::_1),
			&_app->threadPool
		);

	}


	Music music = getApp()->getAudioEngine().loadMusic("Sounds/JPEGSnow.ogg");
	music.play(-1);


	_resourceManager.getGLSLProgram("framebuffer")->use();


	glUniform1i(glGetUniformLocation(_resourceManager.getGLSLProgram("framebuffer")->getProgramID(), "screenTexture"), 0);
	_resourceManager.getGLSLProgram("framebuffer")->unuse();

	_framebuffer.init(_app->_window.getScreenWidth(), _app->_window.getScreenHeight());

	std::string rectInterpolation_str = "RectInterpolation";
	for (NodeEntity* node_entity : manager->getGroup<NodeEntity>(Manager::groupNodes_0)) {
		node_entity->GetComponent<RectangleFlashAnimatorComponent>().Play(rectInterpolation_str);
	}


	//ImGuiIO& io = ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void Graph::onExit() {
	Graph::_PlaneModelRenderer.dispose();
	Graph::_hudPlaneModelRenderer.dispose();
	Graph::_LineRenderer.dispose();
	Graph::_PlaneColorRenderer.dispose();
	Graph::_LightRenderer.dispose();

	_resourceManager.disposeGLSLPrograms();
}


glm::vec2 convertScreenToWorld(glm::vec2 screenCoords) {
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	return main_camera2D->convertScreenToWorld(screenCoords);
}

void Graph::update(float deltaTime) //game objects updating
{
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	glm::vec2 mouseCoordsVec = _sceneMousePosition;

	main_camera2D->update();
	hud_camera2D->update();
	
	if (!manager) {
		return;
	}
	manager->refresh(main_camera2D.get());

	/*glm::vec3 cameraAimPos = main_camera2D->getAimPos();

	glm::vec3 directionToCamera = glm::normalize(cameraAimPos - main_camera2D->eyePos);

	glm::vec3 cameraEulerAngles = main_camera2D->getEulerAnglesFromDirection(directionToCamera);
		*/
	if (_firstLoop) {
		manager->updateFully(deltaTime);
		_firstLoop = false;
	}
	else {
		std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

		manager->update(deltaTime);
	}

	for (auto& node : manager->getVisibleGroup<NodeEntity>(Manager::groupColliders)) {
		node->GetComponent<ColliderComponent>().collisionPhysics();
	}

	if (manager->last_arrowheadsEnabled != manager->arrowheadsEnabled) {
		manager->last_arrowheadsEnabled = manager->arrowheadsEnabled;

		if (manager->arrowheadsEnabled) {

			//todo add to all nodes ports
			for (auto& node : manager->getGroup<NodeEntity>(manager->grid->getGridLevel() == Grid::Level::Basic ? Manager::groupNodes_0 : 
				(manager->grid->getGridLevel() == Grid::Level::Outer1 ? Manager::groupGroupNodes_0 :
					Manager::groupGroupNodes_1
					))) 
			{
				node->addPorts();
			}

			//todo change each links from and to entities (empty entitites - ports)
			for (auto& link : manager->getGroup<LinkEntity>(manager->grid->getGridLevel() == Grid::Level::Basic ? Manager::Manager::groupLinks_0 : 
				(manager->grid->getGridLevel() == Grid::Level::Outer1 ? Manager::Manager::groupGroupLinks_0 :
					Manager::groupGroupLinks_1
					))) 
			{
				link->updateLinkToPorts();
				link->addArrowHead();
			}
		}
		if (!manager->arrowheadsEnabled) {
			//todo change each links from and to entities (from ports, to center of nodes)
			for (auto& link : manager->getGroup<LinkEntity>(Manager::groupLinks_0)) {
				link->updateLinkToNodes();
				link->removeArrowHead();
			}
			for (auto& link : manager->getGroup<LinkEntity>(Manager::groupGroupLinks_0)) {
				link->updateLinkToNodes();
				link->removeArrowHead();
			}
			for (auto& link : manager->getGroup<LinkEntity>(Manager::groupGroupLinks_1)) {
				link->updateLinkToNodes();
				link->removeArrowHead();
			}
			//todo remove all ports
			for (auto& node : manager->getGroup<NodeEntity>(Manager::groupNodes_0)) {
				node->removePorts();
			}
			for (auto& node : manager->getGroup<NodeEntity>(Manager::groupGroupNodes_0)) {
				node->removePorts();
			}
			for (auto& node : manager->getGroup<NodeEntity>(Manager::groupGroupNodes_1)) {
				node->removePorts();
			}
		}
		manager->aboutTo_updateActiveEntities();

	}

	if (_editorImgui.last_activeLayout < _editorImgui.activeLayout) {
		_editorImgui.last_activeLayout += 1;

		manager->grid->setGridLevel(static_cast<Grid::Level>(manager->grid->getGridLevel() + 1));

		if (manager->grid->getGridLevel() == Grid::Level::Outer1) {
			_assetsManager->createGroupLayout(Grid::Level::Outer1);
		}
		else if (manager->grid->getGridLevel() == Grid::Level::Outer2) {
			_assetsManager->createGroupLayout(Grid::Level::Outer2);
		}
	}

	if (_editorImgui.last_activeLayout > _editorImgui.activeLayout) {
		_editorImgui.last_activeLayout -= 1;

		if (manager->grid->getGridLevel() == Grid::Level::Outer1) {
			_assetsManager->ungroupLayout(Grid::Level::Outer1);
		}
		else if (manager->grid->getGridLevel() == Grid::Level::Outer2) {
			_assetsManager->ungroupLayout(Grid::Level::Outer2);
		}

		manager->grid->setGridLevel(static_cast<Grid::Level>(manager->grid->getGridLevel() - 1));


	}


	if (_editorImgui.interpolation_running) {
		_editorImgui.interpolation += _editorImgui.interpolation_speed * deltaTime / _app->getFPSLimiter().fps;
		if (_editorImgui.interpolation >= 1.0f) {
			_editorImgui.interpolation = 0.0f;
		}
	}

	//for all nodes and for all links, get interpolation and accordingly modify the animators?
	if (_editorImgui.interpolation_running) {

		for (NodeEntity* node_entity : manager->getVisibleGroup<NodeEntity>(Manager::groupNodes_0)) {
			if (node_entity->hasComponent<Rectangle_w_Color>()) {
				node_entity->GetComponent<Rectangle_w_Color>().flash_animation.interpolation_a = _editorImgui.interpolation;
					node_entity->GetComponent<Rectangle_w_Color>().setFlashFrame();
				}
			}
			
		for (LinkEntity* link_entity : manager->getVisibleGroup<LinkEntity>(Manager::groupLinks_0)) {
			if (link_entity->hasComponent<Line_w_Color>()) {
				link_entity->GetComponent<Line_w_Color>().flash_animation.interpolation_a = _editorImgui.interpolation;
				link_entity->GetComponent<Line_w_Color>().setFlashFrame();
			}
		}
		
	}

	// check input manager if left mouse is clicked, if yes and the mouse is not on the widget then nullify displayedEntity
	if (_app->_inputManager.isKeyPressed(SDL_BUTTON_LEFT))
	{
		if (!_editorImgui.isMouseOnWidget("Node Display") &&
			!_editorImgui.isMouseOnWidget("Link Display") &&
			!_editorImgui.isMouseOnWidget("Empty Display") &&
			!_editorImgui.isMouseOnWidget("Scene Manager")) {
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

	float stepSize = manager->grid->getCellSize() * 0.5f; // Step size for ray traversal
	glm::vec3 step = glm::normalize(rayDirection) * stepSize; // Step vector
	glm::vec3 currentPos = rayOrigin;

	float traveledDistance = 0.0f;

	while (traveledDistance < maxDistance) {
		int x = static_cast<int>(floor(currentPos.x / (manager->grid->getCellSize() * manager->grid->getLevelCellScale())));
		int y = static_cast<int>(floor(currentPos.y / (manager->grid->getCellSize() * manager->grid->getLevelCellScale())));
		int z = static_cast<int>(floor(currentPos.z / (manager->grid->getCellSize() * manager->grid->getLevelCellScale())));

		// Check if the cell is within the grid bounds
		if (
			x < ceil(manager->grid->getNumXCells() / 2.0f) && x >= ceil(-manager->grid->getNumXCells() / 2.0f) &&
			y < ceil(manager->grid->getNumYCells() / 2.0f) && y >= ceil(-manager->grid->getNumYCells() / 2.0f) &&
			z < ceil(manager->grid->getNumZCells() / 2.0f) && z >= ceil(-manager->grid->getNumZCells() / 2.0f)
			) {
			Cell* cell = manager->grid->getCell(x, y, z, manager->grid->getGridLevel());

			if (cell && visitedCells.find(cell) == visitedCells.end()) {
				hitCells.push_back(cell);
				visitedCells.insert(cell);

				// Add adjacent cells
				for (Cell* adjCell : manager->grid->getAdjacentCells(x, y, z, manager->grid->getGridLevel())) {
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

	if (!_editorImgui.isMouseInSecondColumn) {
		return;
	}

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	std::vector<Cell*> trav_cells = traversedCellsFromRay(rayOrigin, rayDirection, 10000.0f);
	
	bool hasSelected = false;

	glm::vec3 pointAtMaxDepth = main_camera2D->getPointOnRayAtZ(
		rayOrigin,
		rayDirection,
		manager->grid->getNumZCells() * manager->grid->getCellSize() / 2.0f);

	float maxT = glm::distance(rayOrigin, pointAtMaxDepth);
	if (maxT > 10000.0f) maxT = 10000.0f;

	for (auto& trav_cell : trav_cells) {
		for (auto& node : trav_cell->nodes) {
			glm::vec3 t;
			TransformComponent* tempBod = &node->GetComponent<TransformComponent>();
			if (rayIntersectsBox(rayOrigin,
				rayDirection,
				glm::vec3(tempBod->position.x, tempBod->position.y, node->GetComponent<TransformComponent>().getPosition().z),
				glm::vec3(tempBod->position.x + tempBod->size.x, tempBod->position.y + tempBod->size.y, node->GetComponent<TransformComponent>().getPosition().z + tempBod->size.z),
				t,
				maxT)) {
				//std::cout << "Ray hit node: " << node->getId() << " at distance " << t.x << t.y << t.z << std::endl;
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
		for (auto& empty : trav_cell->emptyEntities) {
			glm::vec3 t;
			TransformComponent* tempBod = &empty->GetComponent<TransformComponent>();
			if (rayIntersectsBox(rayOrigin,
				rayDirection,
				glm::vec3(tempBod->position.x, tempBod->position.y, empty->GetComponent<TransformComponent>().getPosition().z),
				glm::vec3(tempBod->position.x + tempBod->size.x, tempBod->position.y + tempBod->size.y, empty->GetComponent<TransformComponent>().getPosition().z + tempBod->size.z),
				t,
				maxT)) {
				//std::cout << "Ray hit empty: " << empty->getId() << " at distance " << t.x << t.y << t.z << std::endl;
				if (activateMode == SDL_BUTTON_RIGHT)
				{
					_displayedEntity = empty;
				}
				else if (activateMode == SDL_BUTTON_LEFT) {
					auto it = std::find_if(_selectedEntities.begin(), _selectedEntities.end(),
						[empty](const std::pair<Entity*, glm::vec3>& entry) {
							return entry.first == empty;
						});

					if (it == _selectedEntities.end()) { // Empty not found
						_selectedEntities.clear();
						_selectedEntities.emplace_back(empty, empty->GetComponent<TransformComponent>().getPosition() - t);
					}
					else {
						std::vector<std::pair<Entity*, glm::vec3>> updatedSelection;
						updatedSelection.reserve(_selectedEntities.size());

						for (const auto& [entity, _] : _selectedEntities) {
							Empty* emptyEntity = dynamic_cast<Empty*>(entity);
							if (emptyEntity) {
								glm::vec3 relativePos = emptyEntity->GetComponent<TransformComponent>().getPosition() - t;
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
					_onHoverEntity = empty;
				}
				else if (activateMode == CTRLD_LEFT_CLICK) {

					auto it = std::find_if(_selectedEntities.begin(), _selectedEntities.end(),
						[empty](const std::pair<Entity*, glm::vec3>& entry) {
							return entry.first == empty;
						});


					//! update selectedEntities relative positions to center
					std::vector<std::pair<Entity*, glm::vec3>> updatedSelection;
					updatedSelection.reserve(_selectedEntities.size());

					for (const auto& [entity, _] : _selectedEntities) {
						Empty* emptyEntity = dynamic_cast<Empty*>(entity);
						if (emptyEntity) {
							glm::vec3 relativePos = emptyEntity->GetComponent<TransformComponent>().getCenterTransform() - t;
							updatedSelection.emplace_back(entity, relativePos);
						}
						else {
							updatedSelection.emplace_back(entity, glm::vec3(0));
						}
					}

					if (it == _selectedEntities.end()) {
						glm::vec3 newEmptyRelativePos = empty->GetComponent<TransformComponent>().getCenterTransform() - t;
						updatedSelection.emplace_back(empty, newEmptyRelativePos);
					}

					_selectedEntities = std::move(updatedSelection);
				}

				hasSelected = true;
				break;
			}
		}

		if (hasSelected) return;
	}
	
	glm::vec3 pointAtMinDepth = main_camera2D->getPointOnRayAtZ(
		rayOrigin,
		rayDirection,
		-manager->grid->getNumZCells() * manager->grid->getCellSize() / 2.0f);

	float minT = glm::distance(rayOrigin, pointAtMinDepth);
	if (minT < 0.0f) minT = 0.0f;

	float sphereRad = 5.0f;
	for (float t = 0.0f; t < minT; t += sphereRad) {
		sphereRad += 0.005f;
	}
	
	for (auto& trav_cell : trav_cells) {

		for (auto& link : trav_cell->links) {
			glm::vec3 t;

			if (rayIntersectsLineSegment(rayOrigin,
				rayDirection,
				link->getFromNode()->GetComponent<TransformComponent>().getCenterTransform(),
				link->getToNode()->GetComponent<TransformComponent>().getCenterTransform(),
				t,
				minT,
				maxT,
				sphereRad)
				) {
				//std::cout << "Ray hit link: " << link->getId() << " at distance " << t.x << t.y << t.z << std::endl;

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

bool Graph::setManager(std::string m_managerName)
{
	bool managerIsNew = false;

	if (m_managerName.empty()) {
		_editorImgui.setNewMap(true);
		int counter = 1;
		while (managers.find("Unnamed_" + std::to_string(counter) + ".txt") != managers.end()) {
			counter++;
		}
		m_managerName = "Unnamed_" + std::to_string(counter) + ".txt";
	}

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	if (!m_managerName.empty() && managers.find(m_managerName) != managers.end()) {
		managerIsNew = false;
	}
	else {
		managerIsNew = true;
	}

	IScene::setManager(m_managerName);
	manager->setThreader(_app->threadPool);

	if (!manager->grid)
	{
		manager->grid = std::make_unique<Grid>(ROW_CELL_SIZE, COLUMN_CELL_SIZE, DEPTH_CELL_SIZE, CELL_SIZE);
		manager->setComponentNames();
	}

	if (!_assetsManager) {
		_assetsManager = new AssetManager(manager, _app->_inputManager, _app->_window);
	}
	_assetsManager->manager = manager;

	if (!map) {
		Graph::map = new Map(*manager, 1, 32);
	}
	map->manager = manager;

	main_camera2D->makeCameraDirty();
	manager->aboutTo_updateActiveEntities();

	return managerIsNew;
}

void Graph::checkInput() {
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	if (!manager) {
		return;
	}
	
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
				main_camera2D->movePosition_Forward(CELL_SIZE);
			}
			else if (evnt.wheel.y < 0)
			{
				// Scrolling down
				main_camera2D->movePosition_Forward(-CELL_SIZE);
			}
			break;
		case SDL_KEYDOWN:
			if (_app->_inputManager.isKeyDown(SDLK_ESCAPE)) {
				if (_displayedEntity || _sceneManagerActive) {
					_displayedEntity = nullptr;
					_sceneManagerActive = false;
				}
				else {
					//_app->exitSimulator();
				}
			}
			if (_displayedEntity) {
				return;
			}
			if (_app->_inputManager.isKeyDown(SDLK_e)) {
				main_camera2D->movePosition_Forward(manager->grid->getCellSize());
			}
			if (_app->_inputManager.isKeyDown(SDLK_r)) {
				main_camera2D->movePosition_Forward(-manager->grid->getCellSize());
			}
			if (_app->_inputManager.isKeyDown(SDLK_w)) {
				main_camera2D->movePosition_Vert(manager->grid->getCellSize() + 10.0f);
			}
			if (_app->_inputManager.isKeyDown(SDLK_s)) {
				main_camera2D->movePosition_Vert(-manager->grid->getCellSize() - 10.0f);
			}
			if (_app->_inputManager.isKeyDown(SDLK_a)) {
				main_camera2D->movePosition_Hor(-manager->grid->getCellSize() - 10.0f);
			}
			if (_app->_inputManager.isKeyDown(SDLK_d)) {
				main_camera2D->movePosition_Hor(manager->grid->getCellSize() + 10.0f);
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
				if (manager->grid->getGridLevel() == Grid::Level::Basic) {
					for (NodeEntity* node_entity : manager->getGroup<NodeEntity>(Manager::groupNodes_0)) {
						if (node_entity->hasComponent<Rectangle_w_Color>()) {
							int alpha = 255;
							node_entity->GetComponent<Rectangle_w_Color>().color.a = alpha;
						}
					}
					for (LinkEntity* link_entity : manager->getVisibleGroup<LinkEntity>(Manager::groupLinks_0)) {
						if (link_entity->hasComponent<Line_w_Color>()) {
							int alpha = 255;
							link_entity->GetComponent<Line_w_Color>().src_color.a = alpha;
							link_entity->GetComponent<Line_w_Color>().dest_color.a = alpha;
						}
					}
				}
				else if (manager->grid->getGridLevel() == Grid::Level::Outer1) {
					for (NodeEntity* node_entity : manager->getGroup<NodeEntity>(Manager::groupGroupNodes_0)) {
						if (node_entity->hasComponent<Rectangle_w_Color>()) {
							int alpha = 255;
							node_entity->GetComponent<Rectangle_w_Color>().color.a = alpha;
						}
					}
					for (LinkEntity* link_entity : manager->getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_0)) {
						if (link_entity->hasComponent<Line_w_Color>()) {
							int alpha = 255;
							link_entity->GetComponent<Line_w_Color>().src_color.a = alpha;
							link_entity->GetComponent<Line_w_Color>().dest_color.a = alpha;
						}
					}
				}
				else if (manager->grid->getGridLevel() == Grid::Level::Outer2) {
					for (NodeEntity* node_entity : manager->getGroup<NodeEntity>(Manager::groupGroupNodes_1)) {
						if (node_entity->hasComponent<Rectangle_w_Color>()) {
							int alpha = 255;
							node_entity->GetComponent<Rectangle_w_Color>().color.a = alpha;
						}
					}
					for (LinkEntity* link_entity : manager->getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_1)) {
						if (link_entity->hasComponent<Line_w_Color>()) {
							int alpha = 255;
							link_entity->GetComponent<Line_w_Color>().src_color.a = alpha;
							link_entity->GetComponent<Line_w_Color>().dest_color.a = alpha;
						}
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
				if (manager->grid->getGridLevel() == Grid::Level::Basic) {

					for (NodeEntity* node_entity : manager->getVisibleGroup<NodeEntity>(Manager::groupNodes_0)) {
						if (node_entity->hasComponent<Rectangle_w_Color>()) {
							int alpha = (connectedEntities.empty() || connectedEntities.count(node_entity)) ? 255 : 100;
							node_entity->GetComponent<Rectangle_w_Color>().color.a = alpha;
						}
					}
					for (LinkEntity* link_entity : manager->getVisibleGroup<LinkEntity>(Manager::groupLinks_0)) {
						if (link_entity->hasComponent<Line_w_Color>()) {
							int alpha = (connectedEntities.empty() || connectedEntities.count(link_entity)) ? 255 : 100;
							link_entity->GetComponent<Line_w_Color>().src_color.a = alpha;
							link_entity->GetComponent<Line_w_Color>().dest_color.a = alpha;
						}
					}
				}
				else if (manager->grid->getGridLevel() == Grid::Level::Outer1) {

					for (NodeEntity* node_entity : manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_0)) {
						if (node_entity->hasComponent<Rectangle_w_Color>()) {
							int alpha = (connectedEntities.empty() || connectedEntities.count(node_entity)) ? 255 : 100;
							node_entity->GetComponent<Rectangle_w_Color>().color.a = alpha;
						}
					}
					for (LinkEntity* link_entity : manager->getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_0)) {
						if (link_entity->hasComponent<Line_w_Color>()) {
							int alpha = (connectedEntities.empty() || connectedEntities.count(link_entity)) ? 255 : 100;
							link_entity->GetComponent<Line_w_Color>().src_color.a = alpha;
							link_entity->GetComponent<Line_w_Color>().dest_color.a = alpha;
						}
					}
				}
				else if (manager->grid->getGridLevel() == Grid::Level::Outer2) {

					for (NodeEntity* node_entity : manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_1)) {
						if (node_entity->hasComponent<Rectangle_w_Color>()) {
							int alpha = (connectedEntities.empty() || connectedEntities.count(node_entity)) ? 255 : 100;
							node_entity->GetComponent<Rectangle_w_Color>().color.a = alpha;
						}
					}
					for (LinkEntity* link_entity : manager->getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_1)) {
						if (link_entity->hasComponent<Line_w_Color>()) {
							int alpha = (connectedEntities.empty() || connectedEntities.count(link_entity)) ? 255 : 100;
							link_entity->GetComponent<Line_w_Color>().src_color.a = alpha;
							link_entity->GetComponent<Line_w_Color>().dest_color.a = alpha;
						}
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
					Empty* emptyEntity = dynamic_cast<Empty*>(entity);
					if (emptyEntity) {
						center += emptyEntity->GetComponent<TransformComponent>().getCenterTransform();
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
					Empty* emptyEntity = dynamic_cast<Empty*>(entity);
					if (emptyEntity) {
						entity->GetComponent<TransformComponent>().setPosition_X(pointAtCenterAxis.x + relativePos.x);
						entity->GetComponent<TransformComponent>().setPosition_Y(pointAtCenterAxis.y + relativePos.y);
					}
				}
			}

			if (_app->_inputManager.isKeyDown(SDL_BUTTON_MIDDLE)) {
				// Calculate new camera position based on the mouse movement
				glm::vec3 delta = glm::vec3(_app->_inputManager.calculatePanningDelta(mouseCoordsVec ), 0.0f);
				main_camera2D->moveAimPos(main_camera2D->getPanningAimPos(), delta);
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
				main_camera2D->setPanningAimPos(main_camera2D->getAimPos());
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
			//onPauseGraph();
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

	_editorImgui.FPSCounter(getApp()->getFPSLimiter());
	ImGui::BeginChild("Tab 1");

	_editorImgui.LeftColumnUIElement(_renderDebug, _clusterLayout, _sceneMousePosition, _app->_inputManager.getMouseCoords(), *manager, _onHoverEntity, _backgroundColor, CELL_SIZE);
	
	ImGui::EndChild();

	ImGui::NextColumn();
	
	std::vector<std::string> openTabs;
	for (const auto& [name, _] : managers) {
		openTabs.push_back(name);
	}

	std::string activeManagerKey = managerName;

	std::string selectedTab = _editorImgui.SceneTabs(openTabs, activeManagerKey);
	if (selectedTab != managerName) {
		setManager(selectedTab);
	}

	_editorImgui.updateIsMouseInSecondColumn();

	_editorImgui.SceneViewport(_framebuffer._framebufferTexture, _windowPos, _windowSize);


	ImGui::NextColumn();
	ImGui::BeginChild("Tab 2");


	_editorImgui.RightColumnUIElement(*manager, &nodeRadius);

	
	ImGui::EndChild();

	ImGui::End();


	_editorImgui.scriptResultsVisualization(*manager, _selectedEntities);
	
	if (_editorImgui.isSaving()) {
		_editorImgui.SavingUI(map);
	}
	if (_editorImgui.isStartingNew()) {
		_editorImgui.NewMapUI();

		if (!_editorImgui.isStartingNew()) {
			std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

			float spacing = 120.0f; // Space between nodes

			float totalWidth = (_editorImgui.newNodesCount - 1) * spacing;
			float startX = -totalWidth * 0.5f;
			float y = 0.0f;

			for (int i = 0; i < _editorImgui.newNodesCount; ++i) {
				auto& node = manager->addEntity<Node>();
				glm::vec2 position = glm::vec2(startX + i * spacing, y);
				node.addComponent<TransformComponent>(position, Layer::action, glm::vec3(10.0f), 1);
				node.addComponent<Rectangle_w_Color>();
				node.GetComponent<Rectangle_w_Color>().color = Color(0, 0, 224, 255);

				node.addGroup(Manager::groupNodes_0);

				manager->grid->addNode(&node, manager->grid->getGridLevel());
			}
			for (int i = 0; i < _editorImgui.newLinksCount; ++i) {
				auto& link = manager->addEntity<Link>( 0 , i + 1);
				link.addComponent<Line_w_Color>();

				link.GetComponent<Line_w_Color>().setSrcColor(Color(255, 40, 0, 255));
				link.GetComponent<Line_w_Color>().setDestColor(Color(40, 255, 0, 255));

				link.addComponent<LineFlashAnimatorComponent>();

				link.addGroup(Manager::groupLinks_0);

				manager->grid->addLink(&link, manager->grid->getGridLevel());
			}

			main_camera2D->makeCameraDirty();
			manager->aboutTo_updateActiveEntities();
		}
	}
	if (_editorImgui.isLoading()) {
		char* loadMapPath = _editorImgui.LoadingUI();
		if (!_editorImgui.isLoading()) {

			if (setManager(std::string(loadMapPath)))
			{
				auto& world_map(manager->addEntityNoId<Empty>());
				_assetsManager->CreateWorldMap(world_map);

				map->loadMap(
					loadMapPath,
					std::bind(&Map::AddDefaultNode, map, std::placeholders::_1, std::placeholders::_2),
					std::bind(&Map::AddDefaultLink, map, std::placeholders::_1),
					&_app->threadPool
				);
			}
		}
	}
	if (_editorImgui.isGoingBack()) {
		_currentState = SceneState::CHANGE_PREVIOUS;
		_editorImgui.SetGoingBack(false);
	}

	
	//glm::vec2 worldToVieport
	if (manager) {
		_editorImgui.ShowEntityComponents(_savedMainViewportMousePosition, _displayedEntity, *manager);
	}


	if (manager && _sceneManagerActive) {
		_editorImgui.ShowSceneControl(_savedMainViewportMousePosition, *manager);
	}
	// this is going to be shown when right click on scene and no displayEntity shows


	glClearColor(_backgroundColor[0], _backgroundColor[1], _backgroundColor[2], _backgroundColor[3]);
}

void Graph::EndRender() {
	_editorImgui.EndRender();
}

void Graph::renderBatch(const std::vector<LinkEntity*>& entities, LineRenderer& batch) {
		//! activate threads near the end, where we have completed everything else
		if (manager->arrowheadsEnabled) {
			_app->threadPool.parallel(entities.size(), [&](int start, int end) {
				for (int i = start; i < end; i++) {
					assert(entities[i]->hasComponent<Line_w_Color>());
					
					entities[i]->GetComponent<Line_w_Color>().drawWithPorts(i, batch, *Graph::_window);
				}
			});
		}
		else {
			_app->threadPool.parallel(entities.size(), [&](int start, int end) {
				for (int i = start; i < end; i++) {
					assert(entities[i]->hasComponent<Line_w_Color>());
					entities[i]->GetComponent<Line_w_Color>().draw(i, batch, *Graph::_window);
				}
			});
		}
	
}

void Graph::renderBatch(const std::vector<NodeEntity*>& entities, PlaneColorRenderer& batch) {

	_app->threadPool.parallel(entities.size(), [&](int start, int end) {
		for (int i = start; i < end; i++) {
			assert(entities[i]->hasComponent<Rectangle_w_Color>());
			entities[i]->GetComponent<Rectangle_w_Color>().draw(i, batch, *Graph::_window);
		}
	});
}

void Graph::renderBatch(const std::vector<EmptyEntity*>& entities, PlaneColorRenderer& batch) {

	_app->threadPool.parallel(entities.size(), [&](int start, int end) {
		for (int i = start; i < end; i++) {
			entities[i]->draw(i, batch, *Graph::_window);
		}
		});
}

void Graph::renderBatch(const std::vector<NodeEntity*>& entities, PlaneModelRenderer& batch) { 
	// before calling this make sure that reserved the right amount of memory

	for (int i = 0; i < entities.size(); i++) {
		entities[i]->draw(i, batch, *Graph::_window);
	}

}
void Graph::renderBatch(const std::vector<EmptyEntity*>& entities, PlaneModelRenderer& batch) {
	// before calling this make sure that reserved the right amount of memory

	for (int i = 0; i < entities.size(); i++) {
		entities[i]->draw(i, batch, *Graph::_window);
	}
}

void Graph::renderBatch(const std::vector<EmptyEntity*>& entities, LightRenderer& batch) {
	// before calling this make sure that reserved the right amount of memory

	for (int i = 0; i < entities.size(); i++) {
		entities[i]->draw(i, batch, *Graph::_window);
	}
}

void Graph::draw()
{
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	GLSLProgram glsl_texture		= *_resourceManager.getGLSLProgram("texture");
	GLSLProgram glsl_light			= *_resourceManager.getGLSLProgram("light");
	GLSLProgram glsl_lineColor		= *_resourceManager.getGLSLProgram("lineColor");
	GLSLProgram glsl_color			= *_resourceManager.getGLSLProgram("color");
	GLSLProgram glsl_framebuffer	= *_resourceManager.getGLSLProgram("framebuffer");

	_framebuffer.Bind();
	////////////OPENGL USE
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glDisable(GL_LINE_SMOOTH);

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

		/*GLint viewportLoc = glsl_lineColor.getUniformLocation("_viewport");
		glUniform4f(viewportLoc, 0.0f, 0.0f, 800.0f, 640.0f);*/


		std::vector<Cell*> intercectedCells = manager->grid->getIntersectedCameraCells(*main_camera2D);
		
		_LineRenderer.initBatchSquares(4);
		
		_LineRenderer.initBatchBoxes(
			intercectedCells.size() +
			manager->getVisibleGroup<NodeEntity>(Manager::groupNodes_0).size() +
			manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_0).size() +
			manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_1).size() 
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
			manager->getVisibleGroup<NodeEntity>(Manager::groupNodes_0),
			manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_0),
			manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_1) 
			}) {
			
			std::vector<NodeEntity*> groupVec = group;
			
			for (auto& entity : groupVec) {

				if (entity->hasComponent<TransformComponent>())
				{
					TransformComponent* tr = &entity->GetComponent<TransformComponent>();

					glm::vec3 nodeBox_org(tr->getPosition().x, tr->getPosition().y, tr->getPosition().z);
					glm::vec3 nodeBox_size(tr->size.x, tr->size.y, tr->size.z);

					_LineRenderer.drawBox(box_v_index++, nodeBox_org, nodeBox_size, Color(255, 255, 255, 255), 0.0f);  // Drawing each cell in red for visibility

					//_LineRenderer.drawCircle(glm::vec2(tr->position.x, tr->position.y), Color(255, 255, 255, 255), tr->getCenterTransform().x);
					//break;
				}

			}
		}
		
		
		_LineRenderer.end();
		_LineRenderer.renderBatch(main_camera2D->getScale() * 10.0f * (manager->grid->getGridLevel() + 1));
		glsl_lineColor.unuse();

	}

	_LineRenderer.begin();
	_PlaneColorRenderer.begin();
	_PlaneModelRenderer.begin();
	_LightRenderer.begin();
	//! Line Renderer Init
	_LineRenderer.initBatchLines(
		manager->getVisibleGroup<LinkEntity>(Manager::groupLinks_0).size() +
		manager->getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_0).size() +
		manager->getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_1).size()
	);
	//! Color Renderer Init
	_PlaneColorRenderer.initQuadBatch(
		manager->getVisibleGroup<NodeEntity>(Manager::groupNodes_0).size() +
		manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_0).size() +
		manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_1).size()
	);
	_PlaneColorRenderer.initTriangleBatch(
		manager->getVisibleGroup<EmptyEntity>(Manager::groupArrowHeads_0).size()
	);
	

	//! Model Renderer Init
	_PlaneModelRenderer.initQuadBatch(
		manager->getVisibleGroup<NodeEntity>(Manager::groupRenderSprites).size()
	);

	//! Light Renderer Init
	_LightRenderer.initBoxBatch(
		manager->getVisibleGroup<EmptyEntity>(Manager::groupEmpties).size()
	);

	_LightRenderer.initSphereBatch(
		manager->getVisibleGroup<EmptyEntity>(Manager::groupSphereEmpties).size()
	);


	_PlaneColorRenderer.initBatchSize();
	_LineRenderer.initBatchSize();
	_PlaneModelRenderer.initBatchSize();
	_LightRenderer.initBatchSize();

	renderBatch(manager->getVisibleGroup<LinkEntity>(Manager::groupLinks_0), _LineRenderer);

	renderBatch(manager->getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_0), _LineRenderer); // todo add offset render based on previous line rendering
	renderBatch(manager->getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_1), _LineRenderer);
	

	//_LineRenderer.renderBatch(cameraMatrix, 2.0f);

	renderBatch(manager->getVisibleGroup<NodeEntity>(Manager::groupNodes_0), _PlaneColorRenderer);
	renderBatch(manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_0), _PlaneColorRenderer);
	renderBatch(manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_1), _PlaneColorRenderer);

	

	renderBatch(manager->getVisibleGroup<EmptyEntity>(Manager::groupArrowHeads_0), _PlaneColorRenderer);


	renderBatch(manager->getVisibleGroup<EmptyEntity>(Manager::groupRenderSprites), _PlaneModelRenderer);

	renderBatch(manager->getVisibleGroup<NodeEntity>(Manager::groupRenderSprites), _PlaneModelRenderer);

	renderBatch(manager->getVisibleGroup<EmptyEntity>(Manager::groupEmpties), _LightRenderer);
	renderBatch(manager->getVisibleGroup<EmptyEntity>(Manager::groupSphereEmpties), _LightRenderer);


	_resourceManager.setupShader(glsl_color, *main_camera2D);
	GLint pLocation = glsl_color.getUniformLocation("rotationMatrix");
	glUniformMatrix4fv(pLocation, 1, GL_FALSE, glm::value_ptr(rotationMatrix));
	_PlaneColorRenderer.end();
	_PlaneColorRenderer.renderBatch(&glsl_color);
	glsl_color.unuse();


	_resourceManager.setupShader(glsl_lineColor, *main_camera2D);
	
	_LineRenderer.end();
	_LineRenderer.renderBatch(main_camera2D->getScale() * 5.0f);
	glsl_lineColor.unuse();

	_resourceManager.setupShader(glsl_texture, *main_camera2D);
	_PlaneModelRenderer.end();
	_PlaneModelRenderer.renderBatch(_resourceManager.getGLSLProgram("texture"));
	glsl_texture.unuse();

	_resourceManager.setupShader(glsl_light, *main_camera2D);
	_LightRenderer.end();
	_LightRenderer.renderBatch(_resourceManager.getGLSLProgram("light"));
	glsl_light.unuse();

	_LineRenderer.begin();

	size_t nodeCount = std::count_if(_selectedEntities.begin(), _selectedEntities.end(),
		[](const std::pair<Entity*, glm::vec3>& entry) {
			return (dynamic_cast<NodeEntity*>(entry.first) != nullptr) 
				|| (dynamic_cast<EmptyEntity*>(entry.first) != nullptr);
		});

	size_t linkCount = std::count_if(_selectedEntities.begin(), _selectedEntities.end(),
		[](const std::pair<Entity*, glm::vec3>& entry) {
			return dynamic_cast<LinkEntity*>(entry.first) != nullptr;
		});

	linkCount += 2 * AXIS_CELLS + 2;

	_LineRenderer.initBatchLines(
		linkCount
		//+1
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
			Empty* empty = dynamic_cast<Empty*>(_selectedEntities[i].first);
			Link* link = dynamic_cast<Link*>(_selectedEntities[i].first);
			if (node || empty) {
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

	glEnable(GL_LINE_SMOOTH);//!this reduces a bit fps

	float z = 0.0f;

	for (int i = 0; i <= AXIS_CELLS; i++) {
		// Vertical lines (constant X, varying Y)
		glm::vec3 startV((i - AXIS_CELLS / 2.0f) * manager->grid->getCellSize(), -AXIS_CELLS / 2.0f * manager->grid->getCellSize(), z);
		glm::vec3 endV((i - AXIS_CELLS / 2.0f) * manager->grid->getCellSize(), AXIS_CELLS / 2.0f * manager->grid->getCellSize(), z);
		_LineRenderer.drawLine(lineIndex++, startV, endV, Color(255, 255, 255, 64), Color(255, 255, 255, 64));

		// Horizontal lines (constant Y, varying X)
		glm::vec3 startH(-AXIS_CELLS / 2.0f * manager->grid->getCellSize(), (i - AXIS_CELLS / 2.0f) * manager->grid->getCellSize(), z);
		glm::vec3 endH(AXIS_CELLS / 2.0f * manager->grid->getCellSize(), (i - AXIS_CELLS / 2.0f) * manager->grid->getCellSize(), z);
		_LineRenderer.drawLine(lineIndex++, startH, endH, Color(255, 255, 255, 64), Color(255, 255, 255, 64));
	}

	//_LineRenderer.drawLine(lineIndex++, pointAtZ0, pointAtO, Color(0, 0, 0, 255), Color(0, 0, 255, 255));

	_resourceManager.setupShader(glsl_lineColor, *main_camera2D);
	

	_LineRenderer.end();
	_LineRenderer.renderBatch(main_camera2D->getCameraRect().x / main_camera2D->getCameraRect().y);
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