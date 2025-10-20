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

	std::vector<Cell*> trav_cells = traversedCellsFromRay(rayOrigin, rayDirection, SELECT_DISTANCE);

	bool hasSelected = false;

	glm::vec3 pointAtMaxDepth = main_camera2D->getPointOnRayAtZ(
		rayOrigin,
		rayDirection,
		manager->grid->getNumZCells() * manager->grid->getCellSize() / 2.0f);

	float maxT = glm::distance(rayOrigin, pointAtMaxDepth);
	if (maxT > SELECT_DISTANCE) maxT = SELECT_DISTANCE;
	// ! Select Nodes
	for (auto& trav_cell : trav_cells) {
		for (auto& nodeId : trav_cell->nodes) {
			auto* node = manager->getEntityFromId(nodeId);

			glm::vec3 t;
			TransformComponent* tempBod = &node->GetComponent<TransformComponent>();
			if (rayIntersectsBox(rayOrigin,
				rayDirection,
				glm::vec3(tempBod->position.x, tempBod->position.y, tempBod->position.z),
				glm::vec3(tempBod->position.x + tempBod->size.x, tempBod->position.y + tempBod->size.y, tempBod->position.z + tempBod->size.z),
				t)) {

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
	// ! Select Empties
	for (auto& trav_cell : trav_cells) {
		for (auto& emptyId : trav_cell->emptyEntities) {
			auto* empty = manager->getEntityFromId(emptyId);

			glm::vec3 t;
			TransformComponent* tempBod = &empty->GetComponent<TransformComponent>();
			if (rayIntersectsBox(rayOrigin,
				rayDirection,
				glm::vec3(tempBod->position.x, tempBod->position.y, empty->GetComponent<TransformComponent>().getPosition().z),
				glm::vec3(tempBod->position.x + tempBod->size.x, tempBod->position.y + tempBod->size.y, empty->GetComponent<TransformComponent>().getPosition().z + tempBod->size.z),
				t)) {
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

	// Helper function to check if entity is already selected
	auto isEntitySelected = [&](Entity* entity) {
		return std::find_if(_selectedEntities.begin(), _selectedEntities.end(),
			[entity](const std::pair<Entity*, glm::vec3>& entry) {
				return entry.first == entity;
			}) != _selectedEntities.end();
		};


	// Unified link selection handler
	auto handleLinkSelection = [&](Entity* link, int mode) {
		std::vector<Entity*> linksToSelect;

		if (link->getParentEntity() &&
			link->getParentEntity()->hasGroup(Manager::groupPathLinksHolder)) {
			auto& pathLinks =
				link->getParentEntity()->GetComponent<PathLinkerComponent>().pathLinks;

			for (auto& pathLink : pathLinks) {
				linksToSelect.push_back(static_cast<Entity*>(pathLink));

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
		for (auto linkId : trav_cell->links) {
			auto* link = dynamic_cast<LinkEntity*>(manager->getEntityFromId(linkId));

			if (rayIntersectsLineSegment(rayOrigin, rayDirection,
				link->getFromNode()->GetComponent<TransformComponent>().getPosition(),
				link->getToNode()->GetComponent<TransformComponent>().getPosition()
			)) {

				// Check if this link is part of a path and select accordingly
				handleLinkSelection(link, activateMode); // true = select whole path if it's a path link
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
			_isDraggingSelectionBox = false;
			_selectionStartPos = glm::vec2(0);
			_selectionCurrentPos = glm::vec2(0);
			_selectionWindowStartPos = glm::vec2(0);
			_selectionWindowCurrentPos = glm::vec2(0);
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
		case SDL_KEYDOWN: {
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

			float accelerationX = 0.0f;
			float accelerationY = 0.0f;
			float accelerationZ = 0.0f;
			float deltaTime = _app->getFPSLimiter().frameTime / 1000.0f; // Convert to seconds

			cameraMaxVelocity = manager->grid->getCellSize(); // Adjust multiplier as needed

			if (_app->_inputManager.isKeyPressed(SDLK_w) ||
				_app->_inputManager.isKeyPressed(SDLK_s)) {
				cameraVelocityY = 0;
			}
			if (_app->_inputManager.isKeyPressed(SDLK_a) ||
				_app->_inputManager.isKeyPressed(SDLK_d)) {
				cameraVelocityX = 0;
			}
			if (_app->_inputManager.isKeyPressed(SDLK_e) ||
				_app->_inputManager.isKeyPressed(SDLK_r)) {
				cameraVelocityZ = 0;
			}

			// Apply input acceleration
			if (_app->_inputManager.isKeyDown(SDLK_w)) {
				accelerationY += cameraAcceleration;
			}
			else if (_app->_inputManager.isKeyDown(SDLK_s)) {
				accelerationY -= cameraAcceleration;
			}
			else {
				cameraVelocityY = 0;
			}
			if (_app->_inputManager.isKeyDown(SDLK_a)) {
				accelerationX -= cameraAcceleration;
			}
			else if (_app->_inputManager.isKeyDown(SDLK_d)) {
				accelerationX += cameraAcceleration;
			}
			else {
				cameraVelocityX = 0;
			}
			if (_app->_inputManager.isKeyDown(SDLK_e)) {
				accelerationZ += cameraAcceleration;
			}
			else if (_app->_inputManager.isKeyDown(SDLK_r)) {
				accelerationZ -= cameraAcceleration;
			}
			else {
				cameraVelocityZ = 0;
			}

			cameraVelocityX += accelerationX * deltaTime * 0.1f * cameraMaxVelocity;
			cameraVelocityY += accelerationY * deltaTime * 0.1f * cameraMaxVelocity;
			cameraVelocityZ += accelerationZ * deltaTime * 0.1f * cameraMaxVelocity;

			// Clamp velocity to maximum
			cameraVelocityX = std::clamp(cameraVelocityX, -cameraMaxVelocity, cameraMaxVelocity);
			cameraVelocityY = std::clamp(cameraVelocityY, -cameraMaxVelocity, cameraMaxVelocity);
			cameraVelocityZ = std::clamp(cameraVelocityZ, -cameraMaxVelocity, cameraMaxVelocity);

			// Apply movement directly with velocity
			const float minVelocity = 0.01f;

			if (std::abs(cameraVelocityX) > minVelocity) {
				main_camera2D->movePosition_Hor(cameraVelocityX);
			}

			if (std::abs(cameraVelocityY) > minVelocity) {
				main_camera2D->movePosition_Vert(cameraVelocityY);
			}

			if (std::abs(cameraVelocityZ) > minVelocity) {
				main_camera2D->movePosition_Forward(cameraVelocityZ);
			}

			break;
		}
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

			auto resetAlphaForEntities = [](Manager* manager,
				const std::vector<EntityID>& nodeGroup,
				const std::vector<EntityID>& linkGroup)
				{
					for (EntityID nodeId : nodeGroup) {
						auto* node = manager->getEntityFromId(nodeId);
						if (node->hasComponent<Rectangle_w_Color>()) {
							auto& rect = node->GetComponent<Rectangle_w_Color>();
							rect.color.a = rect.default_color.a;
						}
					}

					for (EntityID linkId : linkGroup) {
						auto* link = manager->getEntityFromId(linkId);
						if (link->hasComponent<Line_w_Color>()) {
							auto& line = link->GetComponent<Line_w_Color>();
							line.src_color.a = line.default_src_color.a;
							line.dest_color.a = line.default_dest_color.a;
						}
					}
				};

			if (wasHoveringEntity && !_onHoverEntity) {
				Grid::Level level = manager->grid->getGridLevel();

				switch (level) {
				case Grid::Level::Basic:
					resetAlphaForEntities(
						manager,
						manager->getGroup<NodeEntity>(Manager::groupNodes_0),
						manager->getVisibleGroup<LinkEntity>(Manager::groupLinks_0)
					);
					break;

				case Grid::Level::Outer1:
					resetAlphaForEntities(
						manager,
						manager->getGroup<NodeEntity>(Manager::groupGroupNodes_0),
						manager->getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_0)
					);
					break;

				case Grid::Level::Outer2:
					resetAlphaForEntities(
						manager,
						manager->getGroup<NodeEntity>(Manager::groupGroupNodes_1),
						manager->getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_1)
					);
					break;

				default:
					break;
				}
			}

			std::unordered_set<Entity*> connectedEntities;
			if (_onHoverEntity) {
				// --- Build connected entities set ---
				if (auto* hoveredNode = dynamic_cast<Node*>(_onHoverEntity)) {
					connectedEntities.insert(hoveredNode);
					for (auto* link : hoveredNode->getInLinks()) {
						connectedEntities.insert(link);
						connectedEntities.insert(link->getFromNode());
					}
					for (auto* link : hoveredNode->getOutLinks()) {
						connectedEntities.insert(link);
						connectedEntities.insert(link->getToNode());
					}
				}
				else if (auto* hoveredLink = dynamic_cast<Link*>(_onHoverEntity)) {
					connectedEntities.insert(hoveredLink);
					connectedEntities.insert(hoveredLink->getFromNode());
					connectedEntities.insert(hoveredLink->getToNode());
				}

				// --- Helper lambda to set alpha on entities ---
				auto applyAlpha = [&](const std::vector<EntityID>& nodeIds,
					const std::vector<EntityID>& linkIds)
					{
						for (EntityID nodeId : nodeIds) {
							auto* node = manager->getEntityFromId(nodeId);
							if (node->hasComponent<Rectangle_w_Color>()) {
								auto& rect = node->GetComponent<Rectangle_w_Color>();
								rect.color.a = (connectedEntities.empty() || connectedEntities.count(node))
									? 255 : 100;
							}
						}

						for (EntityID linkId : linkIds) {
							auto* link = manager->getEntityFromId(linkId);
							if (link->hasComponent<Line_w_Color>()) {
								auto& line = link->GetComponent<Line_w_Color>();
								int alpha = (connectedEntities.empty() || connectedEntities.count(link))
									? 255 : 100;
								line.src_color.a = alpha;
								line.dest_color.a = alpha;
							}
						}
					};

				// --- Choose groups by grid level ---
				switch (manager->grid->getGridLevel()) {
				case Grid::Level::Basic:
					applyAlpha(
						manager->getVisibleGroup<NodeEntity>(Manager::groupNodes_0),
						manager->getVisibleGroup<LinkEntity>(Manager::groupLinks_0)
					);
					break;

				case Grid::Level::Outer1:
					applyAlpha(
						manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_0),
						manager->getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_0)
					);
					break;

				case Grid::Level::Outer2:
					applyAlpha(
						manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_1),
						manager->getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_1)
					);
					break;

				default:
					break;
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
	for (EntityID entityId : manager->getGroup<EntityType>(groupId)) {
		auto* entity = manager->getEntityFromId(entityId);
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