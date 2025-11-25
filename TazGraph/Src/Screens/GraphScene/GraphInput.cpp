#include "Graph.h"

void Graph::clearSelectedEntities() {

	for (auto& sel : _selectedEntities) {
		Entity* overlayEnt =
			manager->getEntityFromId(sel.overlayEntityId);

		overlayEnt->destroy();
	}

	_selectedEntities.clear();
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

			if (std::holds_alternative<int>(nodeId) && std::get<int>(nodeId) < 0)
				continue;

			Entity* node = manager->getEntityFromId(nodeId);
			if (!node) continue;

			TransformComponent* tr = &node->GetComponent<TransformComponent>();
			glm::vec3 t;

			if (!rayIntersectsBox(
				rayOrigin,
				rayDirection,
				tr->position,
				tr->position + tr->size,
				t))
			{
				continue;
			}

			// --------------------------------------
			// Primary click (left)
			// --------------------------------------
			if (activateMode == SDL_BUTTON_LEFT)
			{
				// check if selected already
				auto it = std::find_if(
					_selectedEntities.begin(), _selectedEntities.end(),
					[&](const SelectedInfo& info) {
						return info.realEntityId == nodeId;
					});

				if (it == _selectedEntities.end()) {
					// not selected → new single selection
					clearSelectedEntities();

					SelectedInfo info;
					info.realEntityId = nodeId;
					info.relativeOffset = tr->getPosition() - t;
					info.overlayEntityId = -1; // created later in batch building

					_selectedEntities.push_back(info);
				}
				else {
					// already selected → update all offsets
					for (auto& sel : _selectedEntities) {
						Entity* e = manager->getEntityFromId(sel.realEntityId);
						sel.relativeOffset = e->GetComponent<TransformComponent>().getPosition() - t;
					}
				}
			}

			// --------------------------------------
			// Ctrl + Left (multi-select)
			// --------------------------------------
			else if (activateMode == CTRLD_LEFT_CLICK)
			{
				// Check if already selected
				auto it = std::find_if(
					_selectedEntities.begin(), _selectedEntities.end(),
					[&](const SelectedInfo& info) {
						return info.realEntityId == nodeId;
					});

				// Update all offsets first
				for (auto& sel : _selectedEntities) {
					Entity* e = manager->getEntityFromId(sel.realEntityId);
					sel.relativeOffset = e->GetComponent<TransformComponent>().getPosition() - t;
				}

				// Add new if not selected
				if (it == _selectedEntities.end())
				{
					SelectedInfo info;
					info.realEntityId = nodeId;
					info.relativeOffset = tr->getPosition() - t;
					info.overlayEntityId = -1; // batch builder will create overlay
					_selectedEntities.push_back(info);
				}
			}

			// --------------------------------------
			// Right-click = show inspector
			// --------------------------------------
			else if (activateMode == SDL_BUTTON_RIGHT) {
				_displayedEntity = node;
			}

			// --------------------------------------
			// Hover
			// --------------------------------------
			else if (activateMode == ON_HOVER && _selectedEntities.empty()) {
				_onHoverEntity = node;
			}

			return; // STOP search
		}
	}
	// ! Select Empties
	for (auto& trav_cell : trav_cells) {
		for (auto& emptyId : trav_cell->emptyEntities) {

			if (std::holds_alternative<int>(emptyId) && std::get<int>(emptyId) < 0)
				continue;

			Entity* empty = manager->getEntityFromId(emptyId);
			if (!empty) continue;

			TransformComponent* tr = &empty->GetComponent<TransformComponent>();
			glm::vec3 t;

			if (!rayIntersectsBox(
				rayOrigin,
				rayDirection,
				tr->position,
				tr->position + tr->size,
				t))
			{
				continue;
			}

			// --------------------------------------
			// Primary click (left)
			// --------------------------------------
			if (activateMode == SDL_BUTTON_LEFT)
			{
				auto it = std::find_if(
					_selectedEntities.begin(), _selectedEntities.end(),
					[&](const SelectedInfo& info) {
						return info.realEntityId == emptyId;
					});

				if (it == _selectedEntities.end()) {
					clearSelectedEntities();

					SelectedInfo info;
					info.realEntityId = emptyId;
					info.relativeOffset = tr->getPosition() - t;
					info.overlayEntityId = -1;

					_selectedEntities.push_back(info);
				}
				else {
					for (auto& sel : _selectedEntities) {
						Entity* e = manager->getEntityFromId(sel.realEntityId);
						sel.relativeOffset = e->GetComponent<TransformComponent>().getPosition() - t;
					}
				}
			}

			// --------------------------------------
			// Ctrl + Left (multi-select)
			// --------------------------------------
			else if (activateMode == CTRLD_LEFT_CLICK)
			{
				auto it = std::find_if(
					_selectedEntities.begin(), _selectedEntities.end(),
					[&](const SelectedInfo& info) {
						return info.realEntityId == emptyId;
					});

				for (auto& sel : _selectedEntities) {
					Entity* e = manager->getEntityFromId(sel.realEntityId);
					sel.relativeOffset = e->GetComponent<TransformComponent>().getPosition() - t;
				}

				if (it == _selectedEntities.end()) {
					SelectedInfo info;
					info.realEntityId = emptyId;
					info.relativeOffset = tr->getPosition() - t;
					info.overlayEntityId = -1;

					_selectedEntities.push_back(info);
				}
			}

			else if (activateMode == SDL_BUTTON_RIGHT) {
				_displayedEntity = empty;
			}
			else if (activateMode == ON_HOVER && _selectedEntities.empty()) {
				_onHoverEntity = empty;
			}

			return;
		}
	}

	glm::vec3 pointAtMinDepth = main_camera2D->getPointOnRayAtZ(
		rayOrigin,
		rayDirection,
		-manager->grid->getNumZCells() * manager->grid->getCellSize() / 2.0f);

	float minT = glm::distance(rayOrigin, pointAtMinDepth);
	if (minT < 0.0f) minT = 0.0f;

	// Helper function to check if entity is already selected
	auto isEntitySelected = [&](EntityID entity) {
		return std::find_if(_selectedEntities.begin(), _selectedEntities.end(),
			[entity](const SelectedInfo& entry) {
				return entry.realEntityId == entity;
			}) != _selectedEntities.end();
		};


	// Unified link selection handler
	auto handleLinkSelection = [&](EntityID linkId, int mode) {
		std::vector<EntityID> linksToSelect;

		Entity* link = manager->getEntityFromId(linkId);

		if (link->getParentEntity() &&
			link->getParentEntity()->hasGroup(Manager::groupPathLinksHolder)) {
			auto& pathLinks =
				link->getParentEntity()->GetComponent<PathLinkerComponent>().pathLinks;

			for (auto& pathLink : pathLinks) {
				linksToSelect.push_back(pathLink);

			}
		}
		else {
			// Single link selection
			linksToSelect.push_back(linkId);
		}

		// Apply selection based on mode
		switch (mode) {
		case SDL_BUTTON_RIGHT:
			_displayedEntity = link; // Still display just the clicked link for UI purposes
			break;

		case SDL_BUTTON_LEFT:
			clearSelectedEntities(); // Clear previous selection
			for (auto linkToSelect : linksToSelect) {

				if (!isEntitySelected(linkToSelect)) {

					SelectedInfo info;
					info.realEntityId = linkToSelect;
					info.relativeOffset = glm::vec3(0);
					info.overlayEntityId = -1; // created later in batch building

					_selectedEntities.push_back(info);
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
			for (auto linkToSelect : linksToSelect) {
				if (!isEntitySelected(linkToSelect)) {
					SelectedInfo info;
					info.realEntityId = linkToSelect;
					info.relativeOffset = glm::vec3(0);
					info.overlayEntityId = -1; // created later in batch building

					_selectedEntities.push_back(info);
				}
			}
			break;
		}
		};

	// Check individual links in trav_cells (now with path awareness)
	for (auto& trav_cell : trav_cells) {
		for (auto linkId : trav_cell->links) {
			if (std::holds_alternative<int>(linkId) && std::get<int>(linkId) < 0)
				continue;
			auto* link = dynamic_cast<LinkEntity*>(manager->getEntityFromId(linkId));

			if (!link) continue;

			if (rayIntersectsLineSegment(rayOrigin, rayDirection,
				manager->getEntityFromId(link->getFromNode())->GetComponent<TransformComponent>().getPosition(),
				manager->getEntityFromId(link->getToNode())->GetComponent<TransformComponent>().getPosition()
			)) {

				// Check if this link is part of a path and select accordingly
				handleLinkSelection(linkId, activateMode); // true = select whole path if it's a path link
				hasSelected = true;
				break;
			}
		}
		if (hasSelected) return;
	}

	if (!hasSelected && activateMode == SDL_BUTTON_LEFT) {
		clearSelectedEntities();
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
			glm::vec2 windowDimension(getApp()->_window.getScreenWidth(), getApp()->_window.getScreenHeight());
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
					for (auto& linkId : hoveredNode->getInLinks()) {
						auto* link = dynamic_cast<LinkEntity*>(manager->getEntityFromId(linkId));
						connectedEntities.insert(link);
						connectedEntities.insert(manager->getEntityFromId(link->getFromNode()));
					}
					for (auto& linkId : hoveredNode->getOutLinks()) {
						auto* link = dynamic_cast<LinkEntity*>(manager->getEntityFromId(linkId));

						connectedEntities.insert(link);
						connectedEntities.insert(manager->getEntityFromId(link->getToNode()));
					}
				}
				else if (auto* hoveredLink = dynamic_cast<Link*>(_onHoverEntity)) {
					connectedEntities.insert(hoveredLink);
					connectedEntities.insert(manager->getEntityFromId(hoveredLink->getFromNode()));
					connectedEntities.insert(manager->getEntityFromId(hoveredLink->getToNode()));
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

				// ============================================================
				// DRAGGING SELECTED ENTITIES (no selection box active)
				// ============================================================
				if (!_selectedEntities.empty() && !_isDraggingSelectionBox)
				{
					glm::vec3 center(0.0f);
					int count = 0;

					// ---- Compute average center of selected entities ----
					for (const auto& sel : _selectedEntities)
					{
						Entity* entity = manager->getEntityFromId(sel.realEntityId);
						if (!entity) continue;

						center += entity->GetComponent<TransformComponent>().getPosition();
						count++;
					}

					if (count > 0)
						center /= (float)count;

					// Ray projected point at Z = center.z
					glm::vec3 pointAtCenterAxis =
						main_camera2D->getPointOnRayAtZ(rayOrigin, rayDirection, center.z);

					// ---- Move each selected entity using its relative offset ----
					for (auto& sel : _selectedEntities)
					{
						Entity* entity = manager->getEntityFromId(sel.realEntityId);
						if (!entity) continue;

						glm::vec3 newPos = pointAtCenterAxis + sel.relativeOffset;

						auto& tr = entity->GetComponent<TransformComponent>();
						tr.setPosition_X(newPos.x);
						tr.setPosition_Y(newPos.y);
						// Z stays unchanged
					}
				}

				// ============================================================
				// BEGIN OR CONTINUE SELECTION BOX
				// ============================================================
				else if (currentTime - _holdStartTime >= HOLD_TIME_FOR_SELECTION)
				{
					if (!_isDraggingSelectionBox)
					{
						// Begin drag box
						_selectionStartPos = _viewportMousePosition;
						_selectionCurrentPos = _viewportMousePosition;

						_selectionWindowStartPos = _app->_inputManager.getMouseCoords();
						_selectionWindowCurrentPos = _app->_inputManager.getMouseCoords();

						_isDraggingSelectionBox = true;
					}
					else
					{
						// Continue drag box
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
		clearSelectedEntities();
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
				[entityId](const SelectedInfo& pair) {
					return pair.realEntityId == entityId;
				});

			if (it == _selectedEntities.end()) {
				SelectedInfo info;
				info.realEntityId = entityId;
				info.relativeOffset = centerPoint;
				info.overlayEntityId = -1; // created later in batch building

				_selectedEntities.push_back(info);
			}
		}
	}
}