#include "MainMenuScreen.h"



MainMenuScreen::MainMenuScreen()
{
	_sceneIndex = SCENE_INDEX_MAIN_MENU;
}

MainMenuScreen::~MainMenuScreen()
{
	//dtor
}

int MainMenuScreen::getNextSceneIndex() const
{
	return _nextSceneIndex;
}

int MainMenuScreen::getPreviousSceneIndex() const
{
	return _prevSceneIndex;
}

void MainMenuScreen::build()
{

}

void MainMenuScreen::destroy()
{

}

void MainMenuScreen::onEntry()
{
	getApp()->enqueueRenderCommand([this]() {
		getApp()->resourceManager.addGLSLProgram("texture");
		getApp()->resourceManager.addGLSLProgram("color");

		if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
		{
			//InitShaders function from Bengine
			getApp()->resourceManager.getGLSLProgram("texture")->compileAndLinkShaders("Src/Shaders/textureBright.vert", "Src/Shaders/textureBright.frag");
			getApp()->resourceManager.getGLSLProgram("texture")->addAttribute("vertexPosition");
			getApp()->resourceManager.getGLSLProgram("texture")->addAttribute("vertexColor");
			getApp()->resourceManager.getGLSLProgram("texture")->addAttribute("vertexUV");

			getApp()->resourceManager.getGLSLProgram("color")->compileAndLinkShaders("Src/Shaders/colorShading.vert", "Src/Shaders/colorShading.frag");
			getApp()->resourceManager.getGLSLProgram("color")->addAttribute("vertexPosition");
			getApp()->resourceManager.getGLSLProgram("color")->addAttribute("vertexColor");
			getApp()->resourceManager.getGLSLProgram("color")->addAttribute("vertexUV");
		}

		std::cout << "Shaders loaded successfully on render thread!" << std::endl;

		if (TTF_Init() == -1)
		{
			std::cout << "Error : SDL_TTF" << std::endl;
		}
		// Texture Loads
		TextureManager::getInstance().Add_GLTexture("graphnetwork", "assets/Sprites/menuBg.png");
		TextureManager::getInstance().Add_GLTexture("arial", "assets/Fonts/arial_cropped_white.png");

		TextureManager::getInstance().Add_Font("arial", "assets/Fonts/arial.ttf", 14.0f);
		TextureManager::getInstance().Add_Font("bold", "assets/Fonts/Figerona-VF.ttf", 16.0f);
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->Build();
		});

	getApp()->waitForRenderCommand();



	auto& Mainmenubackground(manager->addEntity<Empty>());

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("mainMenu_main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("mainMenu_hud"));

	main_camera2D->init(); // Assuming a screen resolution of 800x600
	main_camera2D->setPosition_X(main_camera2D->getPosition().x /*+ glm::vec2(
		width / 2.0f,
		height / 2.0f
	)*/);
	main_camera2D->setPosition_Y(main_camera2D->getPosition().y);
	main_camera2D->setScale(1.0f);

	hud_camera2D->init(); // Assuming a screen resolution of 800x600
	hud_camera2D->setPosition_X(hud_camera2D->getPosition().x /*+ glm::vec2(
		width / 2.0f,
		height / 2.0f
	)*/);
	hud_camera2D->setPosition_Y(hud_camera2D->getPosition().y);
	hud_camera2D->setScale(1.0f);


	if (!manager->grid)
	{
		manager->grid = std::make_unique<Grid>(ROW_CELL_SIZE, COLUMN_CELL_SIZE, DEPTH_CELL_SIZE, CELL_SIZE);

		Mainmenubackground.addComponent<MainMenuBackground>(&getApp()->_window);
		Mainmenubackground.addGroup(Manager::groupBackgroundLayer);
		manager->grid->addEmpty(&Mainmenubackground, manager->grid->getGridLevel());
	}
}



void MainMenuScreen::onExit()
{

}

void MainMenuScreen::update(float deltaTime)
{
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("mainMenu_main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("mainMenu_hud"));

	manager->update(deltaTime);
	manager->refresh(main_camera2D.get());

	main_camera2D->update();
	hud_camera2D->update();
}


void MainMenuScreen::prepareDraw()
{
	int writeIndex = 1 - activeFrameIndex.load();

	auto& frameData = frameDataBuffers[writeIndex];

	auto& mainmenubackground(manager->getGroup<EmptyEntity>(Manager::groupBackgroundLayer));

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("mainMenu_main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("mainMenu_hud"));

	//////////////////////////////////////
	frameData.batches.clear();
	//! Prepare Frame
	{
		Taz::GECSRenderBatch mainMenuBatch;
		mainMenuBatch.renderer_type = Taz::RenderBatch::RendererType::PlaneModel;
		mainMenuBatch.mesh_type = Taz::RenderBatch::MeshType::Quad;

		mainMenuBatch.shaderName = "texture";
		mainMenuBatch.entities = manager->collectEntities(
			{ Manager::groupBackgroundLayer }
		, Taz::EntityType::Empty);
		mainMenuBatch.count = mainMenuBatch.entities.size();
		frameData.batches.push_back(mainMenuBatch);

	}

	getApp()->planeColorRenderer.begin();
	getApp()->lineRenderer.begin();
	getApp()->planeModelRenderer.begin();
	getApp()->lightRenderer.begin();
	//! Prepare Draw Batches by Frame
	{
		for (const auto& batch : frameData.batches) {
			getApp()->prepareBatch(batch);
		}
	}
	activeFrameIndex.store(writeIndex);
}

void MainMenuScreen::renderDraw()
{
	int readIndex = activeFrameIndex.load();
	auto& frameData = frameDataBuffers[readIndex];
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("mainMenu_main"));

	//! render Frame
	{
		for (const auto& batch : frameData.batches) {
			getApp()->renderBatch(batch, *main_camera2D);
		}
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

void MainMenuScreen::checkInput() {
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	SDL_Event evnt;

	while (SDL_PollEvent(&evnt)) {
		ImGui_ImplSDL2_ProcessEvent(&evnt);
		_app->onSDLEvent(evnt);

		switch (evnt.type)
		{
		case SDL_MOUSEMOTION:
			glm::vec2 mouseCoordsVec = _app->_inputManager.getMouseCoords();
			_app->_inputManager.setMouseCoords(mouseCoordsVec.x * main_camera2D->getCameraDimensions().x / getApp()->_window.getScreenWidth(), mouseCoordsVec.y * main_camera2D->getCameraDimensions().y / getApp()->_window.getScreenHeight());
		}

		if (_app->_inputManager.isKeyPressed(SDL_BUTTON_LEFT)) {
			glm::vec2 mouseCoordsVec = _app->_inputManager.getMouseCoords();
			std::cout << mouseCoordsVec.x << " " << mouseCoordsVec.y << std::endl;
		}


	}
}

void MainMenuScreen::BeginRender() {
	ImGuiInterface::BeginRender();
}

void MainMenuScreen::updateUI(float deltaTime) {
	_mainMenuLayer.update(deltaTime);
}

void MainMenuScreen::drawUI() {
	_mainMenuLayer.setConfig({
		   .onStartClicked = [this]() { MainMenuScreen::onStartSimulator(); },
		   .onExitClicked = [this]() { MainMenuScreen::onExitSimulator(); }
		});


	_mainMenuLayer.OnImGuiRender();

	if (DataManager::getInstance().isLoading())
	{
		_mainMenuLayer.getSubcomponent<LoadingUI>()->setConfig({});
		_mainMenuLayer.getSubcomponent<LoadingUI>()->OnImGuiRender();
		char* loadMapPath = DataManager::getInstance().data.input;
		if (strlen(loadMapPath) && !DataManager::getInstance().isLoading()) {
			DataManager::getInstance().mapToLoad = loadMapPath;
			_nextSceneIndex = SCENE_INDEX_GRAPHPLAY;
			currentState = SceneState::CHANGE_NEXT;
		}
	}
	if (!getApp()->openFile.empty()) {
		DataManager::getInstance().mapToLoad = getApp()->openFile;
		_nextSceneIndex = SCENE_INDEX_GRAPHPLAY;
		currentState = SceneState::CHANGE_NEXT;
	}
}

void MainMenuScreen::EndRender() {
	ImGuiInterface::EndRender();
}

bool MainMenuScreen::onStartSimulator() {
	_nextSceneIndex = SCENE_INDEX_GRAPHPLAY;
	currentState = SceneState::CHANGE_NEXT;
	return true;
}

bool MainMenuScreen::onResumeSimulator() {
	_prevSceneIndex = SCENE_INDEX_GRAPHPLAY;
	currentState = SceneState::CHANGE_PREVIOUS;
	return true;
}

void MainMenuScreen::onExitSimulator() {
	_app->exitSimulator();
}