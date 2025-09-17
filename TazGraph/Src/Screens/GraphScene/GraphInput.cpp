#include "Graph.h"
#include <AppScene/AppInterface.h>

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
							glm::vec3 relativePos = nodeEntity->GetComponent<TransformComponent>().getPosition() - t;
							updatedSelection.emplace_back(entity, relativePos);
						}
						else {
							updatedSelection.emplace_back(entity, glm::vec3(0));
						}
					}

					if (it == _selectedEntities.end()) {
						glm::vec3 newNodeRelativePos = node->GetComponent<TransformComponent>().getPosition() - t;
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
							glm::vec3 relativePos = emptyEntity->GetComponent<TransformComponent>().getPosition() - t;
							updatedSelection.emplace_back(entity, relativePos);
						}
						else {
							updatedSelection.emplace_back(entity, glm::vec3(0));
						}
					}

					if (it == _selectedEntities.end()) {
						glm::vec3 newEmptyRelativePos = empty->GetComponent<TransformComponent>().getPosition() - t;
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

	// Helper function to check if entity is already selected
	auto isEntitySelected = [&](Entity* entity) {
		return std::find_if(_selectedEntities.begin(), _selectedEntities.end(),
			[entity](const std::pair<Entity*, glm::vec3>& entry) {
				return entry.first == entity;
			}) != _selectedEntities.end();
		};

	// Helper function to find which path a link belongs to
	auto findPathContainingLink = [&](Entity* targetLink) -> std::vector<LinkEntity*>*{
		for (auto& pathHolder : manager->getGroup<EmptyEntity>(Manager::groupPathLinksHolder)) {
			auto& pathLinks = pathHolder->GetComponent<PathLinkerComponent>().pathLinks;

			for (auto* link : pathLinks) {
				if (link == targetLink) {
					return &pathLinks; // Return reference to the path's link vector
				}
			}
		}
		return nullptr; // Link is not part of any path
		};

	// Unified link selection handler
	auto handleLinkSelection = [&](Entity* link, int mode, bool selectWholePath = false) {
		std::vector<Entity*> linksToSelect;

		if (selectWholePath) {
			// Find the path this link belongs to
			auto* pathLinks = findPathContainingLink(link);
			if (pathLinks) {
				// Add all valid links in the path (cast LinkEntity* to Entity*)
				for (auto* pathLink : *pathLinks) {
					if (pathLink) {
						linksToSelect.push_back(static_cast<Entity*>(pathLink));
					}
				}
			}
			else {
				// Not a path link, just select the single link
				linksToSelect.push_back(link);
			}
		}
		else {
			// Single link selection
			linksToSelect.push_back(link);
		}

		// Apply selection based on mode
		switch (mode) {
		case SDL_BUTTON_RIGHT:
			_displayedEntity = link; // Still display just the clicked link for UI purposes
			break;

		case SDL_BUTTON_LEFT:
			_selectedEntities.clear(); // Clear previous selection
			for (auto* linkToSelect : linksToSelect) {
				if (!isEntitySelected(linkToSelect)) {
					_selectedEntities.emplace_back(linkToSelect, glm::vec3(0));
				}
			}
			break;

		case ON_HOVER:
			if (_selectedEntities.empty()) {
				_onHoverEntity = link; // Still hover just the individual link
			}
			break;

		case CTRLD_LEFT_CLICK:
			// Add to selection (multi-select)
			for (auto* linkToSelect : linksToSelect) {
				if (!isEntitySelected(linkToSelect)) {
					_selectedEntities.emplace_back(linkToSelect, glm::vec3(0));
				}
			}
			break;
		}
		};

	// Check individual links in trav_cells (now with path awareness)
	for (auto& trav_cell : trav_cells) {
		for (auto& link : trav_cell->links) {
			glm::vec3 t;
			if (rayIntersectsLineSegment(rayOrigin, rayDirection,
				link->getFromNode()->GetComponent<TransformComponent>().getPosition(),
				link->getToNode()->GetComponent<TransformComponent>().getPosition(),
				t, minT, maxT, sphereRad)) {

				// Check if this link is part of a path and select accordingly
				handleLinkSelection(link, activateMode, true); // true = select whole path if it's a path link
				hasSelected = true;
				break;
			}
		}
		if (hasSelected) return;
	}

	// Check path link holders (existing behavior - select entire path)
	for (auto& pathHolder : manager->getGroup<EmptyEntity>(Manager::groupPathLinksHolder)) {
		auto& pathLinks = pathHolder->GetComponent<PathLinkerComponent>().pathLinks;
		bool hitAny = false;
		glm::vec3 t;

		// Check if ray hits any link in this path
		for (auto* link : pathLinks) {
			if (!link) continue;

			if (rayIntersectsLineSegment(rayOrigin, rayDirection,
				link->getFromNode()->GetComponent<TransformComponent>().getPosition(),
				link->getToNode()->GetComponent<TransformComponent>().getPosition(),
				t, minT, maxT, sphereRad)) {

				hitAny = true;
				break;
			}
		}

		if (hitAny) {
			// Select all links in this path
			for (auto* link : pathLinks) {
				if (!link) continue;
				handleLinkSelection(link, activateMode, false); // false = don't double-process path selection
			}
			hasSelected = true;
			return; // Stop after first hit path holder
		}
	}

	if (!hasSelected && activateMode == SDL_BUTTON_LEFT) {
		_selectedEntities.clear();
	}
}


void Graph::checkInput() {
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));
	std::shared_ptr<OrthoCamera> minimap_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("minimap"));

	if (!manager) {
		return;
	}

	SDL_Event evnt;
	while (SDL_PollEvent(&evnt)) {
		ImGui_ImplSDL2_ProcessEvent(&evnt);
		_app->onSDLEvent(evnt);

		glm::vec2 mouseCoordsVec = _viewportMousePosition; // in graph we have another variable for the worldCoords of mouse

		glm::vec3 rayOrigin = main_camera2D->getPosition(); // Camera position
		glm::vec3 rayDirection = main_camera2D->castRayAt(_viewportMousePosition); // Ray direction

		if (!_graphEditorLayer.getSubcomponent<GraphMiddlePanel>()->
			getSubcomponent<ViewportPanel>()->
			isMouseInSecondColumn) {
			return;
		}

		switch (evnt.type)
		{
		case SDL_MOUSEWHEEL:
			if (_displayedEntity) {
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
			break;

		case SDL_MOUSEMOTION:
		{
			glm::vec2 viewportPos(_viewportPos.x, _viewportPos.y);
			glm::vec2 windowDimension(_window->getScreenWidth(), _window->getScreenHeight());
			glm::vec2 viewportSize(_viewportSize.x, _viewportSize.y);


			_viewportMousePosition = _app->_inputManager.convertWindowToViewportCoords(
				windowDimension,
				viewportPos,
				viewportSize,
				*main_camera2D
			);

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

			if (_app->_inputManager.isKeyDown(SDL_BUTTON_MIDDLE)) {
				// Calculate new camera position based on the mouse movement
				glm::vec3 delta = glm::vec3(_app->_inputManager.calculatePanningDelta(_viewportMousePosition), 0.0f);
				main_camera2D->moveAimPos(main_camera2D->getPanningAimPos(), delta);
			}

			if (_app->_inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
				Uint32 currentTime = SDL_GetTicks();


				if (!_selectedEntities.empty() && !_isDraggingSelectionBox) {
					glm::vec3 pointAtCenterAxis = glm::vec3(0.0f);

					glm::vec3 center(0.0f);
					int nodeEntitiesSize = 0;

					for (const auto& [entity, _] : _selectedEntities) {
						Node* nodeEntity = dynamic_cast<Node*>(entity);
						if (nodeEntity) {
							center += nodeEntity->GetComponent<TransformComponent>().getPosition();
							nodeEntitiesSize++;
						}
						Empty* emptyEntity = dynamic_cast<Empty*>(entity);
						if (emptyEntity) {
							center += emptyEntity->GetComponent<TransformComponent>().getPosition();
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
				else if (currentTime - _holdStartTime >= HOLD_TIME_FOR_SELECTION) {

					if (!_isDraggingSelectionBox)
					{
						_selectionStartPos = _viewportMousePosition;
						_selectionCurrentPos = _viewportMousePosition;

						_selectionWindowStartPos = _app->_inputManager.getMouseCoords();
						_selectionWindowCurrentPos = _app->_inputManager.getMouseCoords();

						_isDraggingSelectionBox = true;
					}
					else {
						_selectionCurrentPos = _viewportMousePosition;
						_selectionWindowCurrentPos = _app->_inputManager.getMouseCoords();

						performFrustumSelection();
					}
				}
			}
		}
		break;
		case SDL_MOUSEBUTTONDOWN:
		{
			if ((_app->_inputManager.isKeyDown(SDLK_RCTRL) || _app->_inputManager.isKeyDown(SDLK_LCTRL)) &&
				_app->_inputManager.isKeyPressed(SDL_BUTTON_LEFT) &&
				!_selectedEntities.empty()
				) {
				selectEntityFromRay(rayOrigin, rayDirection, CTRLD_LEFT_CLICK);

			}
			if (_app->_inputManager.isKeyPressed(SDL_BUTTON_LEFT)) { // this is for selection and moving around nodes
				_holdStartTime = SDL_GetTicks();
				selectEntityFromRay(rayOrigin, rayDirection, SDL_BUTTON_LEFT);
			}

			if (_app->_inputManager.isKeyPressed(SDL_BUTTON_MIDDLE)) {
				_app->_inputManager.setPanningPoint(_viewportMousePosition);
				main_camera2D->setPanningAimPos(main_camera2D->getAimPos());
			}
			if (_app->_inputManager.isKeyPressed(SDL_BUTTON_RIGHT)) {
				std::cout << "right-clicked at: " << _viewportMousePosition.x << " - " << _viewportMousePosition.y << std::endl;

				selectEntityFromRay(rayOrigin, rayDirection, SDL_BUTTON_RIGHT);

				if (!_displayedEntity) {
					_sceneManagerActive = true;
				}

			}

		}
		break;
		case SDL_MOUSEBUTTONUP:
			if (!_app->_inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
				//_selectedEntities = nullptr;
				_isDraggingSelectionBox = false;
				_selectionStartPos = glm::vec2(0);
				_selectionCurrentPos = glm::vec2(0);

				_selectionWindowStartPos = glm::vec2(0);
				_selectionWindowCurrentPos = glm::vec2(0);
			}
		}
	}
}

void Graph::performFrustumSelection() {
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	if (!_isDraggingSelectionBox) return;

	// Create frustum from selection box
	SelectionFrustum frustum;
	bool isValid = frustum.createFromSelectionBox(_selectionStartPos, _selectionCurrentPos, main_camera2D.get());

	if (isValid) {
		_selectedEntities.clear();
		// Select entities based on current grid level
		switch (manager->grid->getGridLevel()) {
		case Grid::Level::Basic:
			selectEntitiesInFrustum<NodeEntity>(Manager::groupNodes_0, frustum);
			break;

		case Grid::Level::Outer1:
			selectEntitiesInFrustum<NodeEntity>(Manager::groupGroupNodes_0, frustum);
			break;

		case Grid::Level::Outer2:
			selectEntitiesInFrustum<NodeEntity>(Manager::groupGroupNodes_1, frustum);
			break;
		}
	}
}

template<typename EntityType>
void Graph::selectEntitiesInFrustum(int groupId, const SelectionFrustum& frustum) {
	for (EntityType* entity : manager->getGroup<EntityType>(groupId)) {
		glm::vec3 centerPoint = entity->template GetComponent<TransformComponent>().getPosition();
		if (isPointInFrustum(centerPoint, frustum)) {
			auto it = std::find_if(_selectedEntities.begin(), _selectedEntities.end(),
				[entity](const std::pair<Entity*, glm::vec3>& pair) {
					return pair.first == entity;
				});

			if (it == _selectedEntities.end()) {
				_selectedEntities.emplace_back(entity, centerPoint); // Store entity and its position
			}
		}
	}
}