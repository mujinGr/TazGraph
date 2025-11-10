#include "Graph.h"

void Graph::update(float deltaTime) //game objects updating
{

	std::string mapName = DataManager::getInstance().mapToLoad;

	if (!mapName.empty() && setManager(mapName)) {
		auto& world_map(manager->addEntityNoId<Empty>());
		AssetManager::CreateWorldMap(world_map);

		manager->resetEntityId();

		map->loadMap(
			DataManager::getInstance().mapToLoad.c_str(),
			std::bind(&AssetManager::AddDefaultNode, std::placeholders::_1, std::placeholders::_2),
			std::bind(&AssetManager::AddDefaultLink, std::placeholders::_1),
			&_app->threadPool
		);
		DataManager::getInstance().mapToLoad = "";
	}


	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));
	std::shared_ptr<OrthoCamera> minimap_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("minimap"));

	main_camera2D->update();
	hud_camera2D->update();
	minimap_camera2D->update();

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
	}
	else {
		manager->update(deltaTime);
	}

	for (auto nodeId : manager->getVisibleGroup<NodeEntity>(Manager::groupColliders)) {
		auto* node = dynamic_cast<NodeEntity*>(manager->getEntityFromId(nodeId));

		node->GetComponent<ColliderComponent>().collisionPhysics();
	}

	const int GRID_WIDTH = manager->grid->getNumXCells();
	const int GRID_HEIGHT = manager->grid->getNumYCells();
	const int GRID_DEPTH = manager->grid->getNumZCells();

	const int Min_WIDTH_CELL = (-GRID_WIDTH + 1) / 2;
	const int Min_HEIGHT_CELL = (-GRID_HEIGHT + 1) / 2;
	const int Min_DEPTH_CELL = (-GRID_DEPTH + 1) / 2;

	const int Max_WIDTH_CELL = (GRID_WIDTH + 1) / 2;
	const int Max_HEIGHT_CELL = (GRID_HEIGHT + 1) / 2;
	const int Max_DEPTH_CELL = (GRID_DEPTH + 1) / 2;


	static float elapsed = 0.0f;
	elapsed += getApp()->getFPSLimiter().fps / 3600.0f;

	if (elapsed >= 60.0f && processingComplete) {
		elapsed = 0.0f;
		needsRefresh = true;
		processingComplete = false;
		currentX = Min_WIDTH_CELL;
		currentY = Min_HEIGHT_CELL;
		currentZ = Min_DEPTH_CELL;
		// Clear all minimap nodes for fresh start
		manager->removeAllEntitiesFromGroup(Manager::groupMinimapNodes);
	}

	// For first two frames (0 and 1), draw minimap normally with all nodes at once
	if (_firstLoop) {
		currentX = Min_WIDTH_CELL;
		currentY = Min_HEIGHT_CELL;
		currentZ = Min_DEPTH_CELL;
		// Clear existing minimap nodes
		manager->removeAllEntitiesFromGroup(Manager::groupMinimapNodes);

		// Process all cells immediately
		for (int z = Min_DEPTH_CELL; z <= Max_DEPTH_CELL; z++) {
			for (int y = Min_HEIGHT_CELL; y <= Max_HEIGHT_CELL; y++) {
				for (int x = Min_WIDTH_CELL; x <= Max_WIDTH_CELL; x++) {
					Cell* cell = manager->grid->getCell(x, y, z, Grid::Basic);
					if (cell != nullptr) {
						for (auto nodeId : cell->nodes) {
							auto* node = manager->getEntityFromId(nodeId);

							if (node != nullptr) {
								auto& transform = node->GetComponent<TransformComponent>();

								auto& mnode = manager->addEntity<Node>();
								mnode.addGroup(Manager::groupMinimapNodes);

								auto& mtrans = mnode.addComponent<TransformComponent>();
								mtrans.position.x = transform.position.x;
								mtrans.position.y = transform.position.y;
								mtrans.size = transform.size * 10.0f;
								mnode.addComponent<Rectangle_w_Color>();
								mnode.GetComponent<Rectangle_w_Color>().setColor(TazColor(0, 250, 0, 255));
							}
						}
					}
				}
			}
		}
	}

	if (needsRefresh && !processingComplete) {
		while (!processingComplete) {
			// Get current cell
			Cell* cell = manager->grid->getCell(currentX, currentY, currentZ, Grid::Basic);

			if (cell != nullptr) {
				// Process all nodes in this cell
				for (auto nodeId : cell->nodes) {
					auto* node = manager->getEntityFromId(nodeId);

					if (node != nullptr) {
						auto& transform = node->GetComponent<TransformComponent>();

						// Create a new entity in the minimap group
						auto& mnode = manager->addEntity<Node>();
						mnode.addGroup(Manager::groupMinimapNodes);

						// Copy/scale transform
						auto& mtrans = mnode.addComponent<TransformComponent>();
						mtrans.position.x = transform.position.x;
						mtrans.position.y = transform.position.y;
						mtrans.size = transform.size * 10.0f; // enlarge only on minimap
						mnode.addComponent<Rectangle_w_Color>();
						mnode.GetComponent<Rectangle_w_Color>().setColor(TazColor(0, 250, 0, 255));
					}
				}
			}

			// Move to next cell
			currentX++;
			if (currentX > ceil(GRID_WIDTH / 2.0f)) {
				currentX = Min_WIDTH_CELL;
				currentY++;
				if (currentY > ceil(GRID_HEIGHT / 2.0f)) {
					currentY = Min_HEIGHT_CELL;
					currentZ++;
					if (currentZ > ceil(GRID_DEPTH / 2.0f)) {
						// We've processed all cells
						processingComplete = true;
						needsRefresh = false;
						currentX = Min_WIDTH_CELL;
						currentY = Min_HEIGHT_CELL;
						currentZ = Min_DEPTH_CELL; // Reset for next cycle
					}
				}
			}
		}
	}

	if (!manager->last_arrowheadsEnabled && manager->arrowheadsEnabled) {
		//todo add to all nodes ports
		for (auto nodeId : manager->getGroup<NodeEntity>(manager->grid->getGridLevel() == Grid::Level::Basic ? Manager::groupNodes_0 :
			(manager->grid->getGridLevel() == Grid::Level::Outer1 ? Manager::groupGroupNodes_0 :
				Manager::groupGroupNodes_1
				)))
		{
			auto* node = dynamic_cast<NodeEntity*>(manager->getEntityFromId(nodeId));
			node->addPorts();
		}

		//todo change each links from and to entities (empty entitites - ports)
		for (auto linkId : manager->getGroup<LinkEntity>(manager->grid->getGridLevel() == Grid::Level::Basic ? Manager::Manager::groupLinks_0 :
			(manager->grid->getGridLevel() == Grid::Level::Outer1 ? Manager::Manager::groupGroupLinks_0 :
				Manager::groupGroupLinks_1
				)))
		{
			auto* link = dynamic_cast<LinkEntity*>(manager->getEntityFromId(linkId));

			link->updateConnection(LinkEntity::ConnectionType::PORT_TO_PORT);
			link->addArrowHead();
		}
	}

	if ((manager->last_arrowheadsEnabled != manager->arrowheadsEnabled) || manager->updateInnerPathLinks) {
		manager->last_arrowheadsEnabled = manager->arrowheadsEnabled;

		if (manager->arrowheadsEnabled) {

			for (auto linkId : manager->getGroup<LinkEntity>(Manager::groupPathLinks))
			{
				auto* link = dynamic_cast<LinkEntity*>(manager->getEntityFromId(linkId));

				link->updateConnection(LinkEntity::ConnectionType::PORT_TO_PORT);
			}

		}
		if (!manager->arrowheadsEnabled) {

			//todo change each links from and to entities (from ports, to center of nodes)
			for (auto linkId : manager->getGroup<LinkEntity>(Manager::groupLinks_0)) {
				auto* link = dynamic_cast<LinkEntity*>(manager->getEntityFromId(linkId));

				link->updateConnection(LinkEntity::ConnectionType::NODE_TO_NODE);
				link->removeArrowHead();
			}
			for (auto linkId : manager->getGroup<LinkEntity>(Manager::groupGroupLinks_0)) {
				auto* link = dynamic_cast<LinkEntity*>(manager->getEntityFromId(linkId));
				link->updateConnection(LinkEntity::ConnectionType::NODE_TO_NODE);
				link->removeArrowHead();
			}
			for (auto linkId : manager->getGroup<LinkEntity>(Manager::groupGroupLinks_1)) {
				auto* link = dynamic_cast<LinkEntity*>(manager->getEntityFromId(linkId));
				link->updateConnection(LinkEntity::ConnectionType::NODE_TO_NODE);
				link->removeArrowHead();
			}

			for (auto linkId : manager->getGroup<LinkEntity>(Manager::groupPathLinks)) {
				auto* link = dynamic_cast<LinkEntity*>(manager->getEntityFromId(linkId));
				link->updateConnection(LinkEntity::ConnectionType::NODE_TO_NODE);
				link->removeArrowHead();
			}
			for (auto linkId : manager->getGroup<LinkEntity>(Manager::groupPathInnerLinks)) {
				auto* link = dynamic_cast<LinkEntity*>(manager->getEntityFromId(linkId));
				link->updateConnection(LinkEntity::ConnectionType::NODE_TO_NODE);
				link->removeArrowHead();
			}
			//todo remove all ports
			for (auto nodeId : manager->getGroup<NodeEntity>(Manager::groupNodes_0)) {
				auto* node = dynamic_cast<NodeEntity*>(manager->getEntityFromId(nodeId));
				node->removePorts();
			}
			for (auto nodeId : manager->getGroup<NodeEntity>(Manager::groupGroupNodes_0)) {
				auto* node = dynamic_cast<NodeEntity*>(manager->getEntityFromId(nodeId));
				node->removePorts();
			}
			for (auto nodeId : manager->getGroup<NodeEntity>(Manager::groupGroupNodes_1)) {
				auto* node = dynamic_cast<NodeEntity*>(manager->getEntityFromId(nodeId));
				node->removePorts();
			}
		}

		if (manager->updateInnerPathLinks) {
			for (auto pathLinkerId : manager->getGroup<EmptyEntity>(Manager::groupPathLinksHolder))
			{
				auto* pathLinker = dynamic_cast<EmptyEntity*>(manager->getEntityFromId(pathLinkerId));

				pathLinker->GetComponent<PathLinkerComponent>().removeInnerLinks();
			}
			if (manager->arrowheadsEnabled) {
				for (auto pathLinkerId : manager->getGroup<EmptyEntity>(Manager::groupPathLinksHolder))
				{
					auto* pathLinker = dynamic_cast<EmptyEntity*>(manager->getEntityFromId(pathLinkerId));
					pathLinker->GetComponent<PathLinkerComponent>().createInnerLinks();
				}
			}
			manager->updateInnerPathLinks = false;
		}

		manager->aboutTo_updateActiveEntities();

	}

	if (last_showGrid && !showGrid) {
		last_showGrid = showGrid;

		manager->removeAllEntitiesFromLinkGroup(Manager::groupGridLinks);
	}
	float z = 0.0f;

	if (!last_showGrid && showGrid) {
		last_showGrid = showGrid;
		for (int i = 0; i <= AXIS_CELLS; i++) {
			// Vertical lines (constant X, varying Y)
			glm::vec3 startV((i - AXIS_CELLS / 2.0f) * manager->grid->getCellSize(), -AXIS_CELLS / 2.0f * manager->grid->getCellSize(), z);
			glm::vec3 endV((i - AXIS_CELLS / 2.0f) * manager->grid->getCellSize(), AXIS_CELLS / 2.0f * manager->grid->getCellSize(), z);

			auto& vert_gridLink = manager->addEntityNoId<Link>(startV, endV);
			vert_gridLink.addComponent<Line_w_Color>();
			vert_gridLink.GetComponent<Line_w_Color>().width = 1.0f;
			vert_gridLink.GetComponent<Line_w_Color>().setSrcColor(TazColor(255, 255, 255, 64));
			vert_gridLink.addGroup(Manager::groupGridLinks);
			manager->grid->addLink(&vert_gridLink, manager->grid->getGridLevel());

			// Horizontal lines (constant Y, varying X)
			glm::vec3 startH(-AXIS_CELLS / 2.0f * manager->grid->getCellSize(), (i - AXIS_CELLS / 2.0f) * manager->grid->getCellSize(), z);
			glm::vec3 endH(AXIS_CELLS / 2.0f * manager->grid->getCellSize(), (i - AXIS_CELLS / 2.0f) * manager->grid->getCellSize(), z);

			auto& hor_gridLink = manager->addEntityNoId<Link>(startH, endH);
			hor_gridLink.addComponent<Line_w_Color>();
			hor_gridLink.GetComponent<Line_w_Color>().width = 1.0f;
			hor_gridLink.GetComponent<Line_w_Color>().setSrcColor(TazColor(255, 255, 255, 64));

			hor_gridLink.addGroup(Manager::groupGridLinks);

			manager->grid->addLink(&hor_gridLink, manager->grid->getGridLevel());

		}
	}

	// check input manager if left mouse is clicked, if yes and the mouse is not on the widget then nullify displayedEntity
	if (_app->_inputManager.isKeyPressed(SDL_BUTTON_LEFT))
	{
		if (!ImGuiInterface::isMouseOnWidget("Node Display") &&
			!ImGuiInterface::isMouseOnWidget("Link Display") &&
			!ImGuiInterface::isMouseOnWidget("Empty Display") &&
			!ImGuiInterface::isMouseOnWidget("Scene Manager")) {
			_displayedEntity = nullptr;
			_sceneManagerActive = false;
		}
	}

	if (_app->_inputManager.isKeyPressed(SDL_BUTTON_RIGHT))
	{
		_graphEditorLayer.getSubcomponent<GraphMiddlePanel>()->
			getSubcomponent<ViewportPanel>()->
			getSubcomponent<SceneControlPanel>()->savedMousePos = getApp()->_inputManager.getMouseCoords();
	}

	if (_firstLoop) {
		_firstLoop = false;

		manager->aboutTo_updateActiveEntities();
	}
}
