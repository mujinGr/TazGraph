
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

auto& cursor(manager.addEntityWithId<Node>(-1));

float nodeRadius = 1.0f;

float rectangleVertices[] =
{
	// Coords    // texCoords
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f,

	 1.0f,  1.0f,  1.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f
};

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
	}

	if (TTF_Init() == -1)
	{
		std::cout << "Error : SDL_TTF" << std::endl;
	}

	//add the textures to our texture library
	TextureManager::getInstance().Add_GLTexture("arial", "assets/Fonts/arial_cropped_white.png");

	Graph::map = new Map(1, 32);

	//main_camera2D->worldDimensions= grid->GetLayerDimensions();
	if (!manager.grid)
	{
		manager.grid = std::make_unique<Grid>(ROW_CELL_SIZE, COLUMN_CELL_SIZE, CELL_SIZE);
		_assetsManager->CreateCursor(cursor);
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

	_resourceManager.disposeGLSLPrograms();
}

auto& nodes(manager.getGroup(Manager::groupNodes_0));
auto& group_nodes(manager.getGroup(Manager::groupGroupNodes_0));

auto& links(manager.getGroup(Manager::groupLinks_0));
auto& group_links(manager.getGroup(Manager::groupGroupLinks_0));

auto& colliders(manager.getGroup(Manager::groupColliders));

auto& cursors(manager.getGroup(Manager::cursorGroup));

glm::vec2 convertScreenToWorld(glm::vec2 screenCoords) {
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	return main_camera2D->convertScreenToWorld(screenCoords);
}

void Graph::update(float deltaTime) //game objects updating
{
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	glm::vec2 mouseCoordsVec = _app->_inputManager.getMouseCoords();

	checkInput(); //handleEvents

	main_camera2D->update();
	hud_camera2D->update();

	manager.refresh(main_camera2D.get());
	if (_firstLoop) {
		manager.updateFully(deltaTime);
		_firstLoop = false;
	}
	else {
		std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

		manager.update(deltaTime);
	}

	for (auto& cursor : cursors) {
		TransformComponent* tr = &cursor->GetComponent<TransformComponent>();
		tr->setPosition_X(convertScreenToWorld(mouseCoordsVec).x - tr->getSizeCenter().x);
		tr->setPosition_Y(convertScreenToWorld(mouseCoordsVec).y - tr->getSizeCenter().y);
		cursor->update(deltaTime);
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
}



void Graph::selectEntityAtPosition(glm::vec2 worldCoords) {
	auto cells = manager.grid->getAdjacentCells(cursor, manager.grid->getGridLevel());
	for (auto cell : cells) {
		for (auto& entity : cell->links) {

			if (checkCircleLineCollision(worldCoords, 5,
				entity->getFromNode()->GetComponent<TransformComponent>().getCenterTransform(),
				entity->getToNode()->GetComponent<TransformComponent>().getCenterTransform())
				) {
				_selectedEntity = entity;  // Store a pointer or reference to the selected entity
				break;
			}

		}
		for (auto& entity : cell->nodes) {
			if (entity->hasGroup(Manager::cursorGroup)) {
				continue;
			}
			TransformComponent* tr = &entity->GetComponent<TransformComponent>();
			glm::vec2 pos = glm::vec2(tr->getPosition().x, tr->getPosition().y);
			// Check if the mouse click is within the entity's bounding box
			//todo call check collision
			if (checkCollision(SDL_FRect{worldCoords.x,worldCoords.y, 0,0 }, tr->bodyDims)) {
				_selectedEntity = entity;  // Store a pointer or reference to the selected entity
				_app->_inputManager.setObjectRelativePos(glm::vec2(worldCoords - pos));
				break;
			}
		}
	}
}

void Graph::checkInput() {
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	_app->_inputManager.update();

	SDL_Event evnt;
	while (SDL_PollEvent(&evnt)) {
		ImGui_ImplSDL2_ProcessEvent(&evnt);
		_app->onSDLEvent(evnt);

		glm::vec2 mouseCoordsVec = _app->_inputManager.getMouseCoords();
		
		switch (evnt.type)
		{
		case SDL_MOUSEWHEEL:
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
			}
			break;
		case SDL_MOUSEMOTION:
		{
			_app->_inputManager.setMouseCoords(
				mouseCoordsVec.x * main_camera2D->getCameraDimensions().x / _window->getScreenWidth(),
				mouseCoordsVec.y * main_camera2D->getCameraDimensions().y / _window->getScreenHeight()
			);

			mouseCoordsVec = _app->_inputManager.getMouseCoords();
			ImVec2 mainViewportSize = ImGui::GetMainViewport()->Size;
			float scaleCameraToViewportX = mainViewportSize.x / main_camera2D->getCameraDimensions().x;
			float scaleCameraToViewportY = mainViewportSize.y / main_camera2D->getCameraDimensions().y;

			_app->_inputManager.setMouseCoords(
				((mouseCoordsVec.x * scaleCameraToViewportX) - _windowPos.x) * main_camera2D->getCameraDimensions().x / _windowSize.x,
				((mouseCoordsVec.y * scaleCameraToViewportY) - _windowPos.y) * main_camera2D->getCameraDimensions().y / _windowSize.y
			);

			mouseCoordsVec = _app->_inputManager.getMouseCoords();

			if (_selectedEntity) {
				_selectedEntity->GetComponent<TransformComponent>().setPosition_X(convertScreenToWorld(mouseCoordsVec).x - _app->_inputManager.getObjectRelativePos().x);
				_selectedEntity->GetComponent<TransformComponent>().setPosition_Y(convertScreenToWorld(mouseCoordsVec).y - _app->_inputManager.getObjectRelativePos().y);
				if (_selectedEntity->hasComponent<GridComponent>())
					_selectedEntity->GetComponent<GridComponent>().updateCollidersGrid();
			}

			if (_app->_inputManager.isKeyDown(SDL_BUTTON_MIDDLE)) {
				// Calculate new camera position based on the mouse movement
				glm::vec2 delta = _app->_inputManager.calculatePanningDelta(mouseCoordsVec / main_camera2D->getScale());
				/*_app->_camera.move(delta);*/
				main_camera2D->setPosition_X(_app->_inputManager.getStartDragPos().x - delta.x);
				main_camera2D->setPosition_Y(_app->_inputManager.getStartDragPos().y - delta.y);
			}
		}
		case SDL_MOUSEBUTTONDOWN:
			if (_app->_inputManager.isKeyPressed(SDL_BUTTON_LEFT)) {
				std::cout << "clicked at: " << mouseCoordsVec.x << " - " << mouseCoordsVec.y << std::endl;
				if (_selectedEntity == nullptr) {
					selectEntityAtPosition(convertScreenToWorld(mouseCoordsVec));
					std::cout << "convertedScreenToWorld: " << convertScreenToWorld(mouseCoordsVec).x << " - " << convertScreenToWorld(mouseCoordsVec).y << std::endl;
				}
			}

			if (_app->_inputManager.isKeyPressed(SDL_BUTTON_MIDDLE)) {
				_app->_inputManager.setPanningPoint(mouseCoordsVec / main_camera2D->getScale());
				_app->_inputManager.setStartDragPos(main_camera2D->getPosition());
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

	_editorImgui.BackGroundUIElement(_renderDebug, _app->_inputManager.getMouseCoords(), manager, _selectedEntity, _backgroundColor, CELL_SIZE);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 350.0f);
	_editorImgui.FPSCounter(getApp()->getFPSLimiter());
	
	ImGui::EndChild();

	ImGui::NextColumn();
	
	_editorImgui.SceneViewport(_framebuffer._framebufferTexture, _windowPos, _windowSize);

	ImGui::NextColumn();
	_editorImgui.ShowAllEntities(manager, nodeRadius);

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

	_editorImgui.SceneTabs();
	glClearColor(_backgroundColor[0], _backgroundColor[1], _backgroundColor[2], _backgroundColor[3]);
}

void Graph::EndRender() {
	_editorImgui.EndRender();
}

void Graph::renderBatch(const std::vector<Entity*>& entities, LineRenderer& batch) {
	batch.initBatch(entities.size());
	for (const auto& entity : entities) {
		entity->draw(batch, *Graph::_window);
	}
}

void Graph::renderBatch(const std::vector<Entity*>& entities, PlaneModelRenderer& batch) { 
	batch.initBatch(entities.size());
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

	
	// Debug Rendering
	if (_renderDebug) {
		_LineRenderer.begin();
		_resourceManager.setupShader(glsl_lineColor, "", *main_camera2D);

		std::vector<Cell*> intercectedCells = manager.grid->getIntersectedCameraCells(*main_camera2D, manager.grid->getGridLevel());
		for (const auto& cell : intercectedCells) {
			glm::vec4 destRect(cell->boundingBox.x, cell->boundingBox.y, cell->boundingBox.w, cell->boundingBox.h);
			_LineRenderer.drawBox(destRect, Color(0, 255, 0, 100), 0.0f);  // Drawing each cell in red for visibility
		}

		for (std::size_t group = Manager::groupBackgroundLayer; group != Manager::buttonLabels; group++) {
			if (group == Manager::groupLinks_0) continue;

			std::vector<Entity*>& groupVec = manager.getVisibleGroup(group);
			for (auto& entity : groupVec) {

				if (entity->hasComponent<TransformComponent>())
				{
					TransformComponent* tr = &entity->GetComponent<TransformComponent>();

					glm::vec4 destRect;
					destRect.x = tr->getPosition().x;
					destRect.y = tr->getPosition().y;
					destRect.z = tr->bodyDims.w;
					destRect.w = tr->bodyDims.h;
					_LineRenderer.drawBox(destRect, Color(255, 255, 255, 255), 0.0f, 0.0f); //todo add angle for drawbox
					//_LineRenderer.drawCircle(glm::vec2(tr->position.x, tr->position.y), Color(255, 255, 255, 255), tr->getCenterTransform().x);
					//break;
				}

			}
		}
		if (_selectedEntity) {
			TransformComponent* tr = &_selectedEntity->GetComponent<TransformComponent>();

			glm::vec4 destRect;
			destRect.x = tr->getPosition().x;
			destRect.y = tr->getPosition().y;
			destRect.z = tr->bodyDims.w;
			destRect.w = tr->bodyDims.h;
			_LineRenderer.drawBox(destRect, Color(255, 255, 0, 255), 0.0f, 0.0f); //todo add angle for drawbox
		}
		_LineRenderer.drawBox(glm::vec4(-ROW_CELL_SIZE / 2, -COLUMN_CELL_SIZE / 2, ROW_CELL_SIZE/2, COLUMN_CELL_SIZE/2), Color(255, 0, 255, 255), 0.0f, 0.0f);
		_LineRenderer.drawBox(glm::vec4(0, -COLUMN_CELL_SIZE / 2, ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2), Color(255, 0, 255, 255), 0.0f, 0.0f);
		_LineRenderer.drawBox(glm::vec4(-ROW_CELL_SIZE / 2, 0, ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2), Color(255, 0, 255, 255), 0.0f, 0.0f);
		_LineRenderer.drawBox(glm::vec4(0, 0, ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2), Color(255, 0, 255, 255), 0.0f, 0.0f);

		_LineRenderer.end();
		_LineRenderer.renderBatch(main_camera2D->getScale() * 10.0f * (manager.grid->getGridLevel() + 1));
		glsl_lineColor.unuse();

	}
	glm::vec4 destRect;
	destRect.x = main_camera2D->getPosition().x - main_camera2D->getCameraDimensions().x / 2;
	destRect.y = main_camera2D->getPosition().y - main_camera2D->getCameraDimensions().y / 2;
	destRect.z = main_camera2D->getCameraDimensions().x;
	destRect.w = main_camera2D->getCameraDimensions().y;

	_LineRenderer.drawBox(destRect, Color(0, 0, 0, 255), 0.0f, 0.0f);

	_LineRenderer.begin();
	_resourceManager.setupShader(glsl_lineColor, "", *main_camera2D);
	
	renderBatch(manager.getVisibleGroup(Manager::groupLinks_0), _LineRenderer);

	renderBatch(manager.getVisibleGroup(Manager::groupGroupLinks_0), _LineRenderer);

	renderBatch(manager.getVisibleGroup(Manager::groupGroupLinks_1), _LineRenderer);

	_LineRenderer.end();
	_LineRenderer.renderBatch(main_camera2D->getScale() * 5.0f);

	glsl_lineColor.unuse();

	//_LineRenderer.renderBatch(cameraMatrix, 2.0f);


	_PlaneModelRenderer.begin();
	_resourceManager.setupShader(glsl_circleColor, "", *main_camera2D);
	GLint radiusLocation = glsl_circleColor.getUniformLocation("u_radius");
	glUniform1f(radiusLocation, nodeRadius);
	renderBatch(manager.getVisibleGroup(Manager::groupNodes_0), _PlaneModelRenderer);
	renderBatch(manager.getVisibleGroup(Manager::groupGroupNodes_0), _PlaneModelRenderer);
	renderBatch(manager.getVisibleGroup(Manager::groupGroupNodes_1), _PlaneModelRenderer);
	renderBatch(manager.getVisibleGroup(Manager::groupArrowHeads_0), _PlaneModelRenderer);

	renderBatch(cursors, _PlaneModelRenderer);
	_PlaneModelRenderer.end();
	_PlaneModelRenderer.renderBatch();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	/*drawHUD(labels, "arial");
	_resourceManager.getGLSLProgram("color")->use();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/

	glBindTexture(GL_TEXTURE_2D, 0);
	glsl_circleColor.unuse();
	///////////////////////////////////////////////////////

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	_framebuffer.Unbind();

	glClear(GL_COLOR_BUFFER_BIT);
}


void Graph::drawHUD(const std::vector<Entity*>& entities, const std::string& textureName) {
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	_resourceManager.setupShader(*_resourceManager.getGLSLProgram("texture"), textureName, *hud_camera2D);
	renderBatch(entities, _hudPlaneModelRenderer);
}

bool Graph::onPauseGraph() {
	_prevSceneIndex = SCENE_INDEX_MAIN_MENU;
	_currentState = SceneState::CHANGE_PREVIOUS;
	return true;
}