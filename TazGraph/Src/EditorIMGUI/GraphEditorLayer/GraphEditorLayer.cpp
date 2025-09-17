#include "GraphEditorLayer.h"

void GraphEditorLayer::OnImGuiRender()
{
	Manager* manager = config.scene->manager;
	glm::vec2 viewportMousePos = config._viewportMousePosition;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::Begin("Main Window", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);

	getSubcomponent<MenuDropdownPanel>()->setConfig({ .scene = this });
	getSubcomponent<MenuDropdownPanel>()->OnImGuiRender();

	ImGui::Columns(3, "mycolumns");

	static bool initializedUIColumns = false; // Flag to ensure widths are set only once
	if (!initializedUIColumns) {
		float totalWidth = ImGui::GetContentRegionAvail().x;
		ImGuiIO& io = ImGui::GetIO();
		ImVec2 whole_content_size = io.DisplaySize;

		ImGui::SetColumnWidth(0, whole_content_size.x * 0.2f);
		ImGui::SetColumnWidth(1, whole_content_size.x * 0.6f);
		ImGui::SetColumnWidth(2, whole_content_size.x * 0.2f);

		initializedUIColumns = true; // Prevents reapplying widths
	}

	/*_fpsCounter.setLimiter(getApp()->getFPSLimiter());*/

	//_fpsCounter.OnImGuiRender();

	ImGui::BeginChild("Tab 1");

	getSubcomponent<GraphLeftPanel>()->setConfig({
		.scene = config.scene,
		.sceneMouseCoords = viewportMousePos,
		});
	getSubcomponent<GraphLeftPanel>()->OnImGuiRender();

	ImGui::SameLine();

	bool leftColumnExpanded = false;
	if (ImGui::SmallButton(leftColumnExpanded ? "<<" : ">>")) {
		leftColumnExpanded = !leftColumnExpanded;
	}

	ImGui::EndChild();

	ImGui::NextColumn();

	getSubcomponent<GraphMiddlePanel>()->setConfig(
		{
			.scene = config.scene,
			.setManager = config.setManager,
			.c_framebuffer = config.viewportFramebuffer,
			.c_minimapFramebuffer = config.minimapFramebuffer,

			.c_windowPos = &_viewportPos,
			.c_windowSize = &_viewportSize,
			.startPos = _selectionWindowStartPos,
			.currPos = _selectionWindowCurrentPos
		}
	);
	getSubcomponent<GraphMiddlePanel>()->OnImGuiRender();

	//std::vector<std::string> openTabs;
	//for (const auto& [name, _] : managers) {
	//	openTabs.push_back(name);
	//}

	ImGui::NextColumn();
	ImGui::BeginChild("Tab 2");

	getSubcomponent<GraphRightPanel>()->setConfig(
		{
			.scene = config.scene,
			.c_selectedEntities = _selectedEntities
		}
	);
	getSubcomponent<GraphRightPanel>()->OnImGuiRender();

	ImGui::EndChild();

	ImGui::End();

	if (DataManager::getInstance().isSaving()) {
		getSubcomponent<MenuDropdownPanel>()->
			getSubcomponent<SavingUI>()->setConfig({
			.c_map = map
				});
		getSubcomponent<MenuDropdownPanel>()->
			getSubcomponent<SavingUI>()->
			OnImGuiRender();
	}
	if (DataManager::getInstance().isStartingNew()) {
		getSubcomponent<MenuDropdownPanel>()->getSubcomponent<NewMapUI>()->OnImGuiRender();

		if (!DataManager::getInstance().isStartingNew()) {
			std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

			float spacing = 120.0f; // Space between nodes

			float totalWidth = (getSubcomponent<MenuDropdownPanel>()->
				getSubcomponent<NewMapUI>()->
				newNodesCount - 1) * spacing;
			float startX = -totalWidth * 0.5f;
			float y = 0.0f;

			for (int i = 0; i < getSubcomponent<MenuDropdownPanel>()->getSubcomponent<NewMapUI>()->newNodesCount; ++i) {
				auto& node = manager->addEntity<Node>();
				glm::vec2 position = glm::vec2(startX + i * spacing, y);
				node.addComponent<TransformComponent>(position, Layer::action, glm::vec3(10.0f), 1);
				node.addComponent<Rectangle_w_Color>();
				node.GetComponent<Rectangle_w_Color>().color = Color(0, 0, 224, 255);

				node.addGroup(Manager::groupNodes_0);

				manager->grid->addNode(&node, manager->grid->getGridLevel());
			}
			for (int i = 0; i < getSubcomponent<MenuDropdownPanel>()->getSubcomponent<NewMapUI>()->newLinksCount; ++i) {
				auto& link = manager->addEntity<Link>(0, i + 1);
				link.addComponent<Line_w_Color>();

				link.GetComponent<Line_w_Color>().setSrcColor(Color(255, 40, 0, 255));
				link.GetComponent<Line_w_Color>().setDestColor(Color(40, 255, 0, 255));

				link.addComponent<LineFlashAnimatorComponent>();

				link.addGroup(Manager::groupLinks_0);

				manager->grid->addLink(&link, manager->grid->getGridLevel());
			}

			main_camera2D->makeCameraDirty();
			manager->aboutTo_updateActiveEntities();
		}
	}
	if (DataManager::getInstance().isLoading())
	{
		getSubcomponent<MenuDropdownPanel>()->getSubcomponent<LoadingUI>()->setConfig({});
		getSubcomponent<MenuDropdownPanel>()->getSubcomponent<LoadingUI>()->OnImGuiRender();
		char* loadMapPath = DataManager::getInstance().data.input;
		if (strlen(loadMapPath) && !DataManager::getInstance().isLoading()) {

			if (config.setManager(std::string(loadMapPath)))
			{
				auto& world_map(manager->addEntityNoId<Empty>());
				AssetManager::CreateWorldMap(world_map);

				map->loadMap(
					loadMapPath,
					std::bind(&AssetManager::AddDefaultNode, std::placeholders::_1, std::placeholders::_2),
					std::bind(&AssetManager::AddDefaultLink, std::placeholders::_1),
					&config.scene->getApp()->threadPool
				);
			}
		}
	}
	if (DataManager::getInstance().isLoadingPath()) {
		DataManager::getInstance().setPathLoading(false);

		std::string loadPathName = DataManager::getInstance().getPathLoading();

		map->loadPaths(loadPathName.c_str(),
			std::bind(&AssetManager::AddDefaultNode, std::placeholders::_1, std::placeholders::_2),
			std::bind(&AssetManager::AddPathLink, std::placeholders::_1),
			nullptr);
		std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

		main_camera2D->makeCameraDirty();
		manager->aboutTo_updateActiveEntities();
	}
	if (DataManager::getInstance().isGoingBack()) {
		config.scene->currentState = SceneState::CHANGE_PREVIOUS;
		DataManager::getInstance().SetGoingBack(false);
	}


	//glm::vec2 worldToVieport
	if (manager) {
		getSubcomponent<GraphMiddlePanel>()->getSubcomponent<ViewportPanel>()->
			getSubcomponent<EntityComponentsControlPanel>()->
			setConfig({
			.scene = config.scene,
			.displayedEntity = _displayedEntity,
				});
		getSubcomponent<GraphMiddlePanel>()->getSubcomponent<ViewportPanel>()->
			getSubcomponent<EntityComponentsControlPanel>()->
			OnImGuiRender();

		getSubcomponent<GraphMiddlePanel>()->getSubcomponent<ViewportPanel>()->
			getSubcomponent<HoverEntityPanel>()->
			setConfig({
			.scene = config.scene,
			.hoveredEntity = _onHoverEntity,
				});
		getSubcomponent<GraphMiddlePanel>()->getSubcomponent<ViewportPanel>()->
			getSubcomponent<HoverEntityPanel>()->
			OnImGuiRender();
	}

	if (manager && _sceneManagerActive) {
		getSubcomponent<GraphMiddlePanel>()->getSubcomponent<ViewportPanel>()->
			getSubcomponent<SceneControlPanel>()->
			setConfig({
			.scene = this
				});
		getSubcomponent<GraphMiddlePanel>()->getSubcomponent<ViewportPanel>()->
			getSubcomponent<SceneControlPanel>()->
			OnImGuiRender();
	}
}
