
/**
 * @file Graph.cpp

*/

#include "Graph.h"
#include <TextureManager/TextureManager.h>
#include <Camera2.5D/CameraManager.h>
#include "../../Map/Map.h"
#include "GECS/Components.h"
#include "../../GECS/ScriptComponents.h"
#include <sstream>
#include <unordered_set>

#undef main

TazGraphEngine::Window* Graph::_window = nullptr;


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
	getApp()->getFPSLimiter().currentFrame = 0;
	std::string mapName = DataManager::getInstance().mapToLoad;

	/////////////////////////////////////////////
	_resourceManager.addGLSLProgram("color");
	_resourceManager.addGLSLProgram("texture");
	_resourceManager.addGLSLProgram("framebuffer");
	_resourceManager.addGLSLProgram("light");


	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));
	std::shared_ptr<OrthoCamera> minimap_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("minimap"));

	_resourceManager.addGLSLProgram("lineColor");
	_resourceManager.addGLSLProgram("wireframeColor");

	main_camera2D->init(); // Assuming a screen resolution of 800x600
	main_camera2D->setPosition_X(main_camera2D->getPosition().x /*+ glm::vec2(
		width / 2.0f,
		height / 2.0f
	)*/);
	main_camera2D->setPosition_Y(main_camera2D->getPosition().y);
	main_camera2D->setScale(1.0f);

	hud_camera2D->init();
	minimap_camera2D->init();

	minimap_camera2D->setPosition_X(main_camera2D->getPosition().x);
	minimap_camera2D->setPosition_Y(main_camera2D->getPosition().y);

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

		_resourceManager.getGLSLProgram("wireframeColor")->compileAndLinkShaders("Src/Shaders/wireFrameColorShading.vert", "Src/Shaders/wireFrameColorShading.gs", "Src/Shaders/wireFrameColorShading.frag");
		_resourceManager.getGLSLProgram("wireframeColor")->addAttribute("vertexPosition");
		_resourceManager.getGLSLProgram("wireframeColor")->addAttribute("vertexColor");

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
		AssetManager::CreateWorldMap(world_map);

		manager->resetEntityId();

		map->loadMap(
			DataManager::getInstance().mapToLoad.c_str(),
			std::bind(&AssetManager::AddDefaultNode, std::placeholders::_1, std::placeholders::_2),
			std::bind(&AssetManager::AddDefaultLink, std::placeholders::_1),
			&_app->threadPool
		);

	}


	Music music = getApp()->getAudioEngine().loadMusic("Sounds/JPEGSnow.ogg");
	music.play(-1);


	_resourceManager.getGLSLProgram("framebuffer")->use();


	glUniform1i(glGetUniformLocation(_resourceManager.getGLSLProgram("framebuffer")->getProgramID(), "screenTexture"), 0);
	_resourceManager.getGLSLProgram("framebuffer")->unuse();

	_viewportFramebuffer.init(_app->_window.getScreenWidth(), _app->_window.getScreenHeight());
	_minimapFramebuffer.init(_app->_window.getScreenWidth(), _app->_window.getScreenHeight());

	/*std::string rectInterpolation_str = "RectInterpolation";
	for (NodeEntity* node_entity : manager->getGroup<NodeEntity>(Manager::groupNodes_0)) {
		node_entity->GetComponent<RectangleFlashAnimatorComponent>().Play(rectInterpolation_str);
	}*/


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


bool Graph::setManager(std::string m_managerName)
{
	bool managerIsNew = false;

	if (m_managerName.empty()) {
		DataManager::getInstance().setNewMap(true);
		int counter = 1;
		while (managers.find("Unnamed_" + std::to_string(counter) + ".txt") != managers.end()) {
			counter++;
		}
		m_managerName = "Unnamed_" + std::to_string(counter) + ".txt";
	}

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> minimap_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("minimap"));

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

	if (!map) {
		Graph::map = new Map(*manager, 1, 32);
	}
	map->manager = manager;

	main_camera2D->makeCameraDirty();
	minimap_camera2D->makeCameraDirty();

	manager->aboutTo_updateActiveEntities();

	return managerIsNew;
}


void Graph::BeginRender() {
	ImGuiInterface::BeginRender();
}


void Graph::EndRender() {
	ImGuiInterface::EndRender();
}


bool Graph::onPauseGraph() {
	_prevSceneIndex = SCENE_INDEX_MAIN_MENU;
	currentState = SceneState::CHANGE_PREVIOUS;
	return true;
}