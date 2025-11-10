/**
 * @file Graph.cpp

*/

#include "Graph.h"
#include "../../Map/Map.h"


#undef main


Graph::Graph()
{
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
	getApp()->enqueueRenderCommand([this]() {
		/////////////////////////////////////////////
		getApp()->resourceManager.addGLSLProgram("color");
		getApp()->resourceManager.addGLSLProgram("texture");
		getApp()->resourceManager.addGLSLProgram("framebuffer");
		getApp()->resourceManager.addGLSLProgram("light");

		getApp()->resourceManager.addGLSLProgram("lineColor");
		getApp()->resourceManager.addGLSLProgram("wireframeColor");

		if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
		{
			std::cout << "Subsystems Initialised..." << std::endl;

			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

			getApp()->resourceManager.getGLSLProgram("color")->compileAndLinkShaders("Src/Shaders/colorShading.vert", "Src/Shaders/colorShading.frag");
			getApp()->resourceManager.getGLSLProgram("color")->addAttribute("vertexPosition");
			getApp()->resourceManager.getGLSLProgram("color")->addAttribute("vertexColor");
			getApp()->resourceManager.getGLSLProgram("color")->addAttribute("vertexUV");

			getApp()->resourceManager.getGLSLProgram("texture")->compileAndLinkShaders("Src/Shaders/textureBright.vert", "Src/Shaders/textureBright.frag");
			getApp()->resourceManager.getGLSLProgram("texture")->addAttribute("vertexPosition");
			getApp()->resourceManager.getGLSLProgram("texture")->addAttribute("vertexColor");
			getApp()->resourceManager.getGLSLProgram("texture")->addAttribute("vertexUV");

			getApp()->resourceManager.getGLSLProgram("framebuffer")->compileAndLinkShaders("Src/Shaders/framebuffer.vert", "Src/Shaders/framebuffer.frag");
			getApp()->resourceManager.getGLSLProgram("framebuffer")->addAttribute("inPos");
			getApp()->resourceManager.getGLSLProgram("framebuffer")->addAttribute("inTexCoords");

			getApp()->resourceManager.getGLSLProgram("light")->compileAndLinkShaders("Src/Shaders/colorLighting.vert", "Src/Shaders/colorLighting.frag");
			getApp()->resourceManager.getGLSLProgram("light")->addAttribute("vertexPosition");
			getApp()->resourceManager.getGLSLProgram("light")->addAttribute("vertexColor");
			getApp()->resourceManager.getGLSLProgram("light")->addAttribute("vertexUV");

			getApp()->resourceManager.getGLSLProgram("lineColor")->compileAndLinkShaders("Src/Shaders/lineColorShading.vert", "Src/Shaders/lineColorShading.gs", "Src/Shaders/lineColorShading.frag");
			getApp()->resourceManager.getGLSLProgram("lineColor")->addAttribute("vertexPosition");
			getApp()->resourceManager.getGLSLProgram("lineColor")->addAttribute("vertexColor");

			getApp()->resourceManager.getGLSLProgram("wireframeColor")->compileAndLinkShaders("Src/Shaders/wireFrameColorShading.vert", "Src/Shaders/wireFrameColorShading.gs", "Src/Shaders/wireFrameColorShading.frag");
			getApp()->resourceManager.getGLSLProgram("wireframeColor")->addAttribute("vertexPosition");
			getApp()->resourceManager.getGLSLProgram("wireframeColor")->addAttribute("vertexColor");


		}

		getApp()->resourceManager.getGLSLProgram("framebuffer")->use();
		glUniform1i(glGetUniformLocation(getApp()->resourceManager.getGLSLProgram("framebuffer")->getProgramID(), "screenTexture"), 0);
		getApp()->resourceManager.getGLSLProgram("framebuffer")->unuse();

		_viewportFramebuffer.init(_app->_window.getScreenWidth(), _app->_window.getScreenHeight());
		_minimapFramebuffer.init(_app->_window.getScreenWidth(), _app->_window.getScreenHeight());

		if (TTF_Init() == -1)
		{
			std::cout << "Error : SDL_TTF" << std::endl;
		}

		//add the textures to our texture library
		TextureManager::getInstance().Add_GLTexture("arial", "assets/Fonts/arial_cropped_white.png");
		TextureManager::getInstance().Add_GLTexture("worldMap", "assets/Sprites/worldMap.png");
		TextureManager::getInstance().Add_GLTexture("play-button", "assets/Sprites/images-removebg-preview.png");
		TextureManager::getInstance().Add_GLTexture("pause-button", "assets/Sprites/pause.png");
		TextureManager::getInstance().Add_GLTexture("treemap", "assets/Sprites/treemap.png");
		TextureManager::getInstance().Add_GLTexture("sauronEye", "assets/Sprites/Eye-of-Sauron.png");
		});
	getApp()->waitForRenderCommand();



	getApp()->getFPSLimiter().currentFrame = 0;
	std::string mapName = DataManager::getInstance().mapToLoad;


	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));
	std::shared_ptr<OrthoCamera> minimap_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("minimap"));


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


	backgroundColor[0] = 0.407f;
	backgroundColor[1] = 0.384f;
	backgroundColor[2] = 0.356f;
	backgroundColor[3] = 1.0f;

	//ImGuiIO& io = ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void Graph::onExit() {
	
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
