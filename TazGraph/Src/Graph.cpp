
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
		if (!_editorImgui.isMouseOnWidget("Display Entity Statistics")) {
			_displayedEntity = nullptr;
		}
	}

}


std::vector<Cell*> Graph::traversedCellsFromRay(
	glm::vec3 rayOrigin,
	glm::vec3 rayDirection,
	float maxDistance
) {

	std::vector<Cell*> hitCells;

	// Convert world position to grid indices
	int x = static_cast<int>(floor(rayOrigin.x / manager.grid->getCellSize()));
	int y = static_cast<int>(floor(rayOrigin.y / manager.grid->getCellSize()));
	int z = static_cast<int>(floor(rayOrigin.z / manager.grid->getCellSize()));

	// Direction sign (-1, 0, 1)
	int stepX = (rayDirection.x > 0) ? 1 : (rayDirection.x < 0 ? -1 : 0);
	int stepY = (rayDirection.y > 0) ? 1 : (rayDirection.y < 0 ? -1 : 0);
	int stepZ = (rayDirection.z > 0) ? 1 : (rayDirection.z < 0 ? -1 : 0);

	// Compute tMax: when ray crosses next grid boundary
	float tMaxX = (stepX > 0) ? ((x + 1) * manager.grid->getCellSize() - rayOrigin.x) / rayDirection.x
		: (x * manager.grid->getCellSize() - rayOrigin.x) / rayDirection.x;
	float tMaxY = (stepY > 0) ? ((y + 1) * manager.grid->getCellSize() - rayOrigin.y) / rayDirection.y
		: (y * manager.grid->getCellSize() - rayOrigin.y) / rayDirection.y;
	float tMaxZ = (stepZ > 0) ? ((z + 1) * manager.grid->getCellSize() - rayOrigin.z) / rayDirection.z
		: (z * manager.grid->getCellSize() - rayOrigin.z) / rayDirection.z;

	// Avoid division by zero
	if (rayDirection.x == 0) tMaxX = FLT_MAX;
	if (rayDirection.y == 0) tMaxY = FLT_MAX;
	if (rayDirection.z == 0) tMaxZ = FLT_MAX;

	// Compute tDelta: how far we move in t for each step
	float tDeltaX = (stepX != 0) ? fabs(manager.grid->getCellSize() / rayDirection.x) : FLT_MAX;
	float tDeltaY = (stepY != 0) ? fabs(manager.grid->getCellSize() / rayDirection.y) : FLT_MAX;
	float tDeltaZ = (stepZ != 0) ? fabs(manager.grid->getCellSize() / rayDirection.z) : FLT_MAX;

	float t = 0; // Current distance traveled
	while (t < maxDistance) {
		// Check which grid cell contains a node
		Cell* cell = manager.grid->getCell(x, y, z, manager.grid->getGridLevel());

		hitCells.push_back(cell);


		if (tMaxX < tMaxY && tMaxX < tMaxZ) {
			x += stepX;
			t = tMaxX;
			tMaxX += tDeltaX;
		}
		else if (tMaxY < tMaxZ) {
			y += stepY;
			t = tMaxY;
			tMaxY += tDeltaY;
		}
		else {
			z += stepZ;
			t = tMaxZ;
			tMaxZ += tDeltaZ;
		}
	}

	return hitCells;
}

void Graph::selectEntityFromRay(glm::vec3 rayOrigin, glm::vec3 rayDirection, int activateMode) {
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	std::vector<Cell*> trav_cells = traversedCellsFromRay(rayOrigin, rayDirection, 10000.0f);
	
	bool hasSelected = false;

	for (auto& trav_cell : trav_cells) {
		for (auto& node : trav_cell->nodes) {
			float t;

			if (rayIntersectsSphere(rayOrigin,
				rayDirection,
				node->GetComponent<TransformComponent>().getCenterTransform(),
				node->GetComponent<TransformComponent>().bodyDims.w,
				t)) {
				std::cout << "Ray hit node: " << node->getId() << " at distance " << t << std::endl;
				if (activateMode == SDL_BUTTON_RIGHT)
				{
					_displayedEntity = node;
				}
				else if (activateMode == SDL_BUTTON_LEFT) {
					_selectedEntity = node;
				}
				hasSelected = true;
				break;

				/*if (activateMode == SDL_BUTTON_RIGHT)
				{
					_displayedEntity = entity;
				}
				else if (activateMode == SDL_BUTTON_LEFT) {
					_selectedEntity = entity;
				}
				_app->_inputManager.setObjectRelativePos(glm::vec2(worldCoords - pos));
				hasSelected = true;*/
			}
		}

		if (hasSelected) break;

		for (auto& link : trav_cell->links) {
			float t;

			/*if (rayIntersectsLineSegment(rayOrigin,
				rayDirection,
				node->GetComponent<TransformComponent>().getCenterTransform(),
				node->GetComponent<TransformComponent>().bodyDims.w,
				t)) {
				std::cout << "Ray hit node: " << node->getId() << " at distance " << t << std::endl;

				hasSelected = true;
				break;
			}*/
		}
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
			if (!_editorImgui.isMouseInSecondColumn) {
				return;
			}
			if (evnt.wheel.y > 0)
			{
				// Scrolling up
				if (main_camera2D->getScale() < main_camera2D->getMaxScale()) {
					main_camera2D->setScale(main_camera2D->getScale() + SCALE_SPEED);
				}
			}
			else if (evnt.wheel.y < 0)
			{
				// Scrolling down
				if (main_camera2D->getScale() > main_camera2D->getMinScale()) {
					main_camera2D->setScale(main_camera2D->getScale() - SCALE_SPEED);
				}
				else {
					main_camera2D->setScale(main_camera2D->getMinScale());
				}
			}
			break;
		case SDL_KEYDOWN:
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

			if (_selectedEntity) {
				Node* node = dynamic_cast<Node*>(_selectedEntity);
				if (node) {

					glm::vec3 pointAtEntity = main_camera2D->getPointOnRayAtZ(rayOrigin, rayDirection, _selectedEntity->GetComponent<TransformComponent>().getZIndex());;

					_selectedEntity->GetComponent<TransformComponent>().setPosition_X(pointAtEntity.x);
					_selectedEntity->GetComponent<TransformComponent>().setPosition_Y(pointAtEntity.y);
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
			if (_app->_inputManager.isKeyPressed(SDL_BUTTON_LEFT)) { // this is for selection and moving around nodes
				std::cout << "clicked at: " << _sceneMousePosition.x << " - " << _sceneMousePosition.y << std::endl;
				if (_selectedEntity == nullptr) {
					/*glm::vec3 ray = castRayAt(_sceneMousePosition);
					selectEntityFromRay(ray);*/
					selectEntityFromRay(rayOrigin, rayDirection, SDL_BUTTON_LEFT);
					
					// Get point on the ray at z = 0
					pointAtZ0 = main_camera2D->getPointOnRayAtZ(rayOrigin, rayDirection, 0.0f);

					// Get point on the ray at z = -100
					pointAtO = main_camera2D->getPointOnRayAtZ(rayOrigin, rayDirection, rayOrigin.z);


					std::cout << "convertedScreenToWorld: " << convertScreenToWorld(mouseCoordsVec).x << " - " << convertScreenToWorld(mouseCoordsVec).y << std::endl;
				}
			}

			if (_app->_inputManager.isKeyPressed(SDL_BUTTON_MIDDLE)) {
				_app->_inputManager.setPanningPoint(mouseCoordsVec);
				_app->_inputManager.setStartDragAimPos(main_camera2D->getAimPos());
			}
			if (_app->_inputManager.isKeyPressed(SDL_BUTTON_RIGHT)) {
				std::cout << "right-clicked at: " << mouseCoordsVec.x << " - " << mouseCoordsVec.y << std::endl;

				selectEntityFromRay(rayOrigin, rayDirection, SDL_BUTTON_RIGHT);

				_savedMainViewportMousePosition = _app->_inputManager.getMouseCoords();
			}
		case SDL_MOUSEBUTTONUP:
			if (!_app->_inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
				_selectedEntity = nullptr;
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
	ImGui::BeginChild("Tab 1");

	_editorImgui.BackGroundUIElement(_renderDebug, _sceneMousePosition, _app->_inputManager.getMouseCoords(), manager, _selectedEntity, _backgroundColor, CELL_SIZE);
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
	_editorImgui.ShowStatisticsAbout(_savedMainViewportMousePosition, _displayedEntity);

	glClearColor(_backgroundColor[0], _backgroundColor[1], _backgroundColor[2], _backgroundColor[3]);
}

void Graph::EndRender() {
	_editorImgui.EndRender();
}

void Graph::renderBatch(size_t startIndex, const std::vector<LinkEntity*>& entities, LineRenderer& batch) {
		for (int i = 0; i < entities.size(); i++) {
			if (entities[i]->hasComponent<Line_w_Color>()) {
				entities[i]->GetComponent<Line_w_Color>().draw(i + startIndex, batch, *Graph::_window);
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
	for (const auto& entity : entities) {
		entity->draw(batch, *Graph::_window);
	}
}
void Graph::renderBatch(const std::vector<EmptyEntity*>& entities, PlaneModelRenderer& batch) {
	// before calling this make sure that reserved the right amount of memory
	for (const auto& entity : entities) {
		entity->draw(batch, *Graph::_window);
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
		_resourceManager.setupShader(glsl_lineColor, "", *main_camera2D);

		std::vector<Cell*> intercectedCells = manager.grid->getIntersectedCameraCells(*main_camera2D);
		
		_LineRenderer.initBatchSquares(1);
		
		_LineRenderer.initBatchBoxes(
			//intercectedCells.size() +
			manager.getVisibleGroup<NodeEntity>(Manager::groupNodes_0).size() +
			manager.getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_0).size() +
			manager.getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_1).size() +
			(_selectedEntity ? 1 : 0)
		);
		

		_LineRenderer.initBatchSize();

		size_t v_index = 0;

		_LineRenderer.drawRectangle(v_index++, glm::vec4(-ROW_CELL_SIZE / 2, -COLUMN_CELL_SIZE / 2, ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2), Color(255, 0, 255, 255), 0.0f, 0.0f);
		//_LineRenderer.drawRectangle(v_index++, glm::vec4(0, -COLUMN_CELL_SIZE / 2, ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2), Color(255, 0, 255, 255), 0.0f, 0.0f);
		//_LineRenderer.drawRectangle(v_index++, glm::vec4(-ROW_CELL_SIZE / 2, 0, ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2), Color(255, 0, 255, 255), 0.0f, 0.0f);
		//_LineRenderer.drawRectangle(v_index++, glm::vec4(0, 0, ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2), Color(255, 0, 255, 255), 0.0f, 0.0f);


		size_t box_v_index = 0;

		for (const auto& cell : intercectedCells) {
			glm::vec3 cellBox_org(cell->boundingBox_origin.x, cell->boundingBox_origin.y, cell->boundingBox_origin.z);
			glm::vec3 cellBox_size(cell->boundingBox_size.x, cell->boundingBox_size.y, cell->boundingBox_size.z);

			//_LineRenderer.drawBox(box_v_index++, cellBox_org, cellBox_size, Color(0, 255, 0, 20), 0.0f);  // Drawing each cell in red for visibility
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
					destRect.z = tr->bodyDims.w;
					destRect.w = tr->bodyDims.h;

					glm::vec3 nodeBox_org(destRect.x, destRect.y, tr->getZIndex());
					glm::vec3 nodeBox_size(destRect.z, destRect.w, destRect.w);

					_LineRenderer.drawBox(box_v_index++, nodeBox_org, nodeBox_size, Color(255, 255, 255, 255), 0.0f);  // Drawing each cell in red for visibility

					//_LineRenderer.drawCircle(glm::vec2(tr->position.x, tr->position.y), Color(255, 255, 255, 255), tr->getCenterTransform().x);
					//break;
				}

			}
		}
		if (_selectedEntity && _selectedEntity->hasComponent<TransformComponent>()) {
				TransformComponent* tr = &_selectedEntity->GetComponent<TransformComponent>();

				glm::vec4 destRect;
				destRect.x = tr->getPosition().x;
				destRect.y = tr->getPosition().y;
				destRect.z = tr->bodyDims.w;
				destRect.w = tr->bodyDims.h;

				glm::vec3 nodeBox_org(destRect.x, destRect.y, tr->getZIndex());
				glm::vec3 nodeBox_size(destRect.z, destRect.w, destRect.w);

				_LineRenderer.drawBox(box_v_index++, nodeBox_org, nodeBox_size, Color(255, 255, 0, 255), 0.0f); //todo add angle for drawRectangle

		}
		
		_LineRenderer.end();
		_LineRenderer.renderBatch(main_camera2D->getScale() * 10.0f * (manager.grid->getGridLevel() + 1));
		glsl_lineColor.unuse();

	}

	_LineRenderer.begin();
	_PlaneColorRenderer.begin();

	_LineRenderer.initBatchLines(
		manager.getVisibleGroup<LinkEntity>(Manager::groupLinks_0).size() +
		manager.getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_0).size() +
		manager.getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_1).size() +
		1
	);
	
	_PlaneColorRenderer.initColorQuadBatch(
		manager.getVisibleGroup<NodeEntity>(Manager::groupNodes_0).size() +
		manager.getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_0).size() +
		manager.getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_1).size()
	);
	_PlaneColorRenderer.initColorTriangleBatch(
		manager.getVisibleGroup<EmptyEntity>(Manager::groupArrowHeads_0).size()
	);

	_PlaneColorRenderer.initBatchSize();
	_LineRenderer.initBatchSize();

	_LineRenderer.drawLine(0, pointAtZ0, pointAtO, Color(0, 0, 0, 255), Color(0, 0, 255, 255));
	
	renderBatch(1, manager.getVisibleGroup<LinkEntity>(Manager::groupLinks_0), _LineRenderer);

	//renderBatch(1, manager.getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_0), _LineRenderer);
	//renderBatch(1, manager.getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_1), _LineRenderer);
	_resourceManager.setupShader(glsl_lineColor, "", *main_camera2D);
	_LineRenderer.end();
	_LineRenderer.renderBatch(main_camera2D->getScale() * 5.0f);

	glsl_lineColor.unuse();

	//_LineRenderer.renderBatch(cameraMatrix, 2.0f);


	_resourceManager.setupShader(glsl_color, "", *main_camera2D);
	
	

	GLint pLocation = glsl_color.getUniformLocation("rotationMatrix");
	glUniformMatrix4fv(pLocation, 1, GL_FALSE, glm::value_ptr(rotationMatrix));
	
	//renderBatch(manager.getVisibleGroup<NodeEntity>(Manager::groupNodes_0), _PlaneColorRenderer);
	//renderBatch(manager.getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_1), _PlaneColorRenderer);
	

	renderBatch(manager.getVisibleGroup<EmptyEntity>(Manager::groupArrowHeads_0), _PlaneColorRenderer);

	_PlaneColorRenderer.end();
	_PlaneColorRenderer.renderBatch(_resourceManager.getGLSLProgram("color"));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	/*drawHUD(labels, "arial");
	_resourceManager.getGLSLProgram("color")->use();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/

	glBindTexture(GL_TEXTURE_2D, 0);
	glsl_color.unuse();
	///////////////////////////////////////////////////////

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	_framebuffer.Unbind();

	glClear(GL_COLOR_BUFFER_BIT);
}


void Graph::drawHUD(const std::vector<NodeEntity*>& entities, const std::string& textureName) {
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	_resourceManager.setupShader(*_resourceManager.getGLSLProgram("texture"), textureName, *hud_camera2D);
	renderBatch(entities, _hudPlaneModelRenderer);
}

bool Graph::onPauseGraph() {
	_prevSceneIndex = SCENE_INDEX_MAIN_MENU;
	_currentState = SceneState::CHANGE_PREVIOUS;
	return true;
}