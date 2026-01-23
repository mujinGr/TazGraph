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

		for (int i = 0; i < 2; i++) {
			auto& frameData = frameDataBuffers[i];

			frameData.planeColorRenderer.init();
			frameData.lineRenderer.init();
			frameData.planeModelRenderer.init();
			frameData.lightRenderer.init();

			auto& minimap_frameData = minimap_frameDataBuffers[i];

			minimap_frameData.planeColorRenderer.init();
			minimap_frameData.lineRenderer.init();
			minimap_frameData.planeModelRenderer.init();
			minimap_frameData.lightRenderer.init();
		}

		getApp()->resourceManager.addGLSLProgram("texture");
		getApp()->resourceManager.addGLSLProgram("color");
		getApp()->resourceManager.addGLSLProgram("framebuffer");

		if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
		{
			TAZ_LOG("Subsystems Initialised...");

			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);


			//InitShaders function from Bengine
			getApp()->resourceManager.getGLSLProgram("texture")->compileAndLinkShaders("Src/Shaders/textureBright.vert", "Src/Shaders/textureBright.frag");
			getApp()->resourceManager.getGLSLProgram("texture")->addAttribute("vertexPosition");
			getApp()->resourceManager.getGLSLProgram("texture")->addAttribute("vertexColor");
			getApp()->resourceManager.getGLSLProgram("texture")->addAttribute("vertexUV");

			getApp()->resourceManager.getGLSLProgram("framebuffer")->compileAndLinkShaders("Src/Shaders/framebuffer.vert", "Src/Shaders/framebuffer.frag");
			getApp()->resourceManager.getGLSLProgram("framebuffer")->addAttribute("inPos");
			getApp()->resourceManager.getGLSLProgram("framebuffer")->addAttribute("inTexCoords");

			getApp()->resourceManager.getGLSLProgram("color")->compileAndLinkShaders("Src/Shaders/colorShading.vert", "Src/Shaders/colorShading.frag");
			getApp()->resourceManager.getGLSLProgram("color")->addAttribute("vertexPosition");
			getApp()->resourceManager.getGLSLProgram("color")->addAttribute("vertexColor");
			getApp()->resourceManager.getGLSLProgram("color")->addAttribute("vertexUV");
		}
		getApp()->resourceManager.getGLSLProgram("framebuffer")->use();
		glUniform1i(
			glGetUniformLocation(getApp()->resourceManager.getGLSLProgram("framebuffer")->getProgramID(), "screenTexture")
			, 0);
		getApp()->resourceManager.getGLSLProgram("framebuffer")->unuse();



		_main_viewportFramebuffer.init(_app->_window.getScreenWidth(), _app->_window.getScreenHeight(), getApp()->useMSAA, getApp()->MSAA_samples);

		if (TTF_Init() == -1)
		{
			TAZ_LOG("Error : SDL_TTF");
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
		Mainmenubackground.addToGroup(Manager::groupBackgroundLayer);
		manager->grid->addEmpty(&Mainmenubackground, manager->grid->getGridLevel());
	}
}



void MainMenuScreen::onExit()
{

}

void MainMenuScreen::update(float deltaTime)
{
	ZoneScopedN("Main-Update");

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("mainMenu_main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("mainMenu_hud"));

	manager->update(deltaTime);
	manager->refresh(main_camera2D.get());

	main_camera2D->update();
	hud_camera2D->update();
}


void MainMenuScreen::prepareDraw(int index)
{
	ZoneScopedN("Main-PrepareDraw");


	int writeIndex = 1 - getApp()->activeIndex.load();

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
		mainMenuBatch.entities = manager->collectVisibleEntities(
			{ Manager::groupBackgroundLayer }
		, Taz::EntityType::Empty);
		mainMenuBatch.count = mainMenuBatch.entities.size();
		frameData.batches.push_back(mainMenuBatch);

	}

	frameData.planeColorRenderer.begin();
	frameData.lineRenderer.begin();
	frameData.planeModelRenderer.begin();
	frameData.lightRenderer.begin();

	{
		for (auto& batch : frameData.batches) {
			getApp()->prepareBatch(batch, frameData);
		}
	}

}

void MainMenuScreen::renderDraw(int index)
{
	ZoneScopedN("Main-RenderDraw");

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("mainMenu_main"));
	int readIndex = getApp()->activeIndex.load();
	auto& frameData = frameDataBuffers[readIndex];

	_main_viewportFramebuffer.Bind();
	glViewport(0, 0, _main_viewportFramebuffer._width, _main_viewportFramebuffer._height);

	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
	//! render Frame
	{
		for (const auto& batch : frameData.batches) {
			getApp()->renderBatch(batch, frameData, *main_camera2D);
		}
	}
	_main_viewportFramebuffer.Unbind();
}

void MainMenuScreen::checkInput() {
	ZoneScopedN("Main-CheckInput");

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	if (requestExit) {
		MainMenuScreen::onExitSimulator();
	}

	SDL_Event evnt;
	while (SDL_PollEvent(&evnt)) {
		{
			std::lock_guard<std::mutex> lock(getApp()->imguiEventsMutex);
			getApp()->imguiEvents.push_back(evnt);
		}
		_app->onSDLEvent(evnt);

		switch (evnt.type)
		{
		case SDL_MOUSEMOTION:
			glm::vec2 mouseCoordsVec = _app->_inputManager.getMouseCoords();
			_app->_inputManager.setMouseCoords(mouseCoordsVec.x * main_camera2D->getCameraDimensions().x / getApp()->_window.getScreenWidth(), mouseCoordsVec.y * main_camera2D->getCameraDimensions().y / getApp()->_window.getScreenHeight());
		}

		if (_app->_inputManager.isKeyPressed(SDL_BUTTON_LEFT)) {
			glm::vec2 mouseCoordsVec = _app->_inputManager.getMouseCoords();
			TAZ_LOG(std::to_string(mouseCoordsVec.x) + " " + std::to_string(mouseCoordsVec.y));
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
	ZoneScopedN("Main-DrawUI");

	_mainMenuLayer.setConfig({
		   .onStartClicked = [this]() { MainMenuScreen::onStartSimulator(); },
		   .onExitClicked = [this]() { requestExit = true; },
		   .viewportFramebuffer = &_main_viewportFramebuffer,
		});

	_mainMenuLayer.OnImGuiRender();

	if (DataManager::getInstance().isLoading())
	{
		_mainMenuLayer.getSubcomponent<LoadingUI>()->setConfig({});
		_mainMenuLayer.getSubcomponent<LoadingUI>()->OnImGuiRender();
		char* loadMapPath = DataManager::getInstance().data.input;
		if (strlen(loadMapPath) && !DataManager::getInstance().isLoading()) {
			DataManager::getInstance().setMapToLoad(loadMapPath);
			_nextSceneIndex = SCENE_INDEX_GRAPHPLAY;
			currentState = SceneState::CHANGE_NEXT;
		}
	}
	if (!getApp()->openFile.empty()) {
		DataManager::getInstance().setMapToLoad(getApp()->openFile);
		_nextSceneIndex = SCENE_INDEX_GRAPHPLAY;
		currentState = SceneState::CHANGE_NEXT;
	}
}

void MainMenuScreen::SwapBufferDraw() {
	ZoneScopedN("Main-SwapBuffer");

	getApp()->_window.swapBuffer();
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
