
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

auto& cursor(manager.addEntity(-1));

float nodeRadius = 0.0f;

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
	_resourceManager.addGLSLProgram("color");
	_resourceManager.addGLSLProgram("circleColor");
	_resourceManager.addGLSLProgram("texture");

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
}

void Graph::onExit() {
	Graph::_PlaneModelRenderer.dispose();
	Graph::_hudPlaneModelRenderer.dispose();
	Graph::_LineRenderer.dispose();

	_resourceManager.disposeGLSLPrograms();
}

auto& nodes(manager.getGroup(Manager::groupNodes_0));
auto& links(manager.getGroup(Manager::groupLinks_0));

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
}



void Graph::selectEntityAtPosition(glm::vec2 worldCoords) {
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	TransformComponent* tr = &cursor.GetComponent<TransformComponent>();
	auto cells = manager.grid->getAdjacentCells(tr->getCenterTransform().x, tr->getCenterTransform().y);
	for (auto cell : cells) {
		for (auto& entity : cell->entities) {
			if (entity->hasGroup(Manager::cursorGroup)) {
				continue;
			}
			TransformComponent* tr = &entity->GetComponent<TransformComponent>();
			glm::vec2 pos = glm::vec2(tr->getPosition().x, tr->getPosition().y);
			// Check if the mouse click is within the entity's bounding box
			if (worldCoords.x > pos.x && worldCoords.x < pos.x + tr->width &&
				worldCoords.y > pos.y && worldCoords.y < pos.y + tr->height) {
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
				if (main_camera2D->getScale() < main_camera2D->getMinScale()) {
					main_camera2D->setScale(main_camera2D->getScale() + SCALE_SPEED);
				}
			}
			else if (evnt.wheel.y < 0)
			{
				// Scrolling down
				if (main_camera2D->getScale() > main_camera2D->getMaxScale()) {
					main_camera2D->setScale(main_camera2D->getScale() - SCALE_SPEED);
				}
			}
			break;
		case SDL_MOUSEMOTION:
			_app->_inputManager.setMouseCoords(mouseCoordsVec.x * main_camera2D->getCameraDimensions().x / _window->getScreenWidth(), mouseCoordsVec.y * main_camera2D->getCameraDimensions().y / _window->getScreenHeight());
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
		case SDL_MOUSEBUTTONDOWN:
			if (_app->_inputManager.isKeyPressed(SDL_BUTTON_LEFT)) {
				std::cout << "clicked at: " << mouseCoordsVec.x << " - " << mouseCoordsVec.y << std::endl;
				if (_selectedEntity == nullptr) {
					selectEntityAtPosition(convertScreenToWorld(mouseCoordsVec));
					std::cout << "convertedScreenToWorld: " << convertScreenToWorld(mouseCoordsVec).x << " - " << convertScreenToWorld(mouseCoordsVec).y << std::endl;
				}
			}
			if (!_app->_inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
				_selectedEntity = nullptr;
			}

			if (_app->_inputManager.isKeyPressed(SDL_BUTTON_MIDDLE)) {
				_app->_inputManager.setPanningPoint(mouseCoordsVec / main_camera2D->getScale());
				_app->_inputManager.setStartDragPos(main_camera2D->getPosition());
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
	_editorImgui.BackGroundUIElement(_renderDebug, _app->_inputManager.getMouseCoords(), manager, _selectedEntity, _backgroundColor, CELL_SIZE);
	_editorImgui.FileActions();
	_editorImgui.FPSCounter(getApp()->getFPSLimiter());
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
	_editorImgui.ShowAllEntities(manager, nodeRadius);
}

void Graph::EndRender() {
	_editorImgui.EndRender();
}

void Graph::renderBatch(const std::vector<Entity*>& entities, LineRenderer& batch) {
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	for (const auto& entity : entities) {
		// todo do culling here
		entity->draw(batch, *Graph::_window);
	}
}

void Graph::renderBatch(const std::vector<Entity*>& entities, PlaneModelRenderer& batch) { 
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	for (const auto& entity : entities) {
		if (entity->hasComponent<Rectangle_w_Color>()) {
			Rectangle_w_Color entityRectangle = entity->GetComponent<Rectangle_w_Color>();
			if (checkCollision(entityRectangle.destRect, main_camera2D->getCameraRect())) { //todo: draw culling, apply this on all entities
				entity->draw(batch, *Graph::_window);
			}
		}
		else {
			entity->draw(batch, *Graph::_window);
		}
	}

	 // todo: instead of rendering on each group, import allentities for renderBatches
}

void Graph::draw()
{
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	GLSLProgram glsl_texture		= *_resourceManager.getGLSLProgram("texture");
	GLSLProgram glsl_lineColor		= *_resourceManager.getGLSLProgram("lineColor");
	GLSLProgram glsl_circleColor	= *_resourceManager.getGLSLProgram("circleColor");
	GLSLProgram glsl_color			= *_resourceManager.getGLSLProgram("color");

	////////////OPENGL USE
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(_backgroundColor[0], _backgroundColor[1], _backgroundColor[2], _backgroundColor[3]);


	/////////////////////////////////////////////////////
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	_resourceManager.setupShader(glsl_lineColor, "", *main_camera2D);

	// Debug Rendering
	if (_renderDebug) {
		std::vector<Cell*> intercectedCells = manager.grid->getIntercectedCameraCells(*main_camera2D);
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
					destRect.z = tr->width;
					destRect.w = tr->height;
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
			destRect.z = tr->width;
			destRect.w = tr->height;
			_LineRenderer.drawBox(destRect, Color(255, 255, 0, 255), 0.0f, 0.0f); //todo add angle for drawbox
		}

		_LineRenderer.end();
		_LineRenderer.renderBatch(main_camera2D->getScale() * 2.0f);
		glsl_lineColor.unuse();

	}
	_resourceManager.setupShader(glsl_lineColor, "", *main_camera2D);

	glm::vec4 destRect;
	destRect.x = main_camera2D->getPosition().x - main_camera2D->getCameraDimensions().x/2;
	destRect.y = main_camera2D->getPosition().y - main_camera2D->getCameraDimensions().y/2;
	destRect.z = main_camera2D->getCameraDimensions().x;
	destRect.w = main_camera2D->getCameraDimensions().y;

	_LineRenderer.drawBox(destRect, Color(0, 0, 0, 255), 0.0f, 0.0f);

	renderBatch(manager.getVisibleGroup(Manager::groupLinks_0), _LineRenderer);
	
	_LineRenderer.end();
	_LineRenderer.renderBatch(main_camera2D->getScale() * 2.0f);
	glsl_lineColor.unuse();

	//_LineRenderer.renderBatch(cameraMatrix, 2.0f);


	_PlaneModelRenderer.begin();
	_resourceManager.setupShader(glsl_circleColor, "", *main_camera2D);
	GLint radiusLocation = glsl_circleColor.getUniformLocation("u_radius");
	glUniform1f(radiusLocation, nodeRadius);
	renderBatch(manager.getVisibleGroup(Manager::groupNodes_0), _PlaneModelRenderer);
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
	_resourceManager.setupShader(glsl_color, "", *main_camera2D);

	glsl_color.unuse();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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