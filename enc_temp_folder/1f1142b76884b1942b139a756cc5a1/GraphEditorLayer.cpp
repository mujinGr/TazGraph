#include "GraphEditorLayer.h"

void GraphEditorLayer::OnImGuiRender()
{
	Manager* manager = config.scene->manager;
	glm::vec2 viewportMousePos = config.viewportMousePosition;

	ImGuiViewport* window = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(window->Pos);
	ImGui::SetNextWindowSize(window->Size);
	ImGui::Begin("Main Window", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);

	getSubcomponent<MenuDropdownPanel>()->setConfig({ .scene = config.scene });
	getSubcomponent<MenuDropdownPanel>()->OnImGuiRender();

	ImGui::BeginTable("MainLayoutTable", 3,
		ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_NoSavedSettings);

	static bool initializedUIColumns = false; // Flag to ensure widths are set only once
	if (!initializedUIColumns) {
		ImGuiIO& io = ImGui::GetIO();
		ImVec2 whole_content_size = io.DisplaySize;

		ImGui::TableSetupColumn("Left", ImGuiTableColumnFlags_WidthFixed, whole_content_size.x * 0.2f);
		ImGui::TableSetupColumn("Mid", ImGuiTableColumnFlags_WidthFixed, whole_content_size.x * 0.6f);
		ImGui::TableSetupColumn("Right", ImGuiTableColumnFlags_WidthFixed, whole_content_size.x * 0.2f);

		initializedUIColumns = true; // Prevents reapplying widths
	}

	float buttonWidth = ImGui::CalcTextSize(">>").x + ImGui::GetStyle().FramePadding.x * 2.0f;

	//if (!leftColumnExpanded)
	//{
	//	ImGui::SetColumnWidth(0, buttonWidth + 4.0f); // for example
	//}
	ImGui::TableNextColumn();

	ImGui::BeginChild("Tab 1");

	if (ImGui::BeginTable("LeftPanelTable", 2, ImGuiTableFlags_SizingFixedFit))
	{
		if (leftColumnExpanded)
			ImGui::TableSetupColumn("LeftPanel", ImGuiTableColumnFlags_WidthStretch);
		else
			ImGui::TableSetupColumn("LeftPanel", ImGuiTableColumnFlags_WidthFixed, 0.0f);

		ImGui::TableSetupColumn("ButtonColumn", ImGuiTableColumnFlags_WidthFixed, buttonWidth);
		ImGui::TableNextColumn();

		if (leftColumnExpanded)
		{
			getSubcomponent<GraphLeftPanel>()->setConfig({
				.scene = config.scene,
				.sceneMouseCoords = viewportMousePos,
				});
			getSubcomponent<GraphLeftPanel>()->OnImGuiRender();
		}
		ImGui::TableNextColumn();


		if (ImGui::Button(leftColumnExpanded ? "<<" : ">>"))
			leftColumnExpanded = !leftColumnExpanded;

		ImGui::EndTable();
	}
	ImGui::EndChild();

	ImGui::TableNextColumn();

	getSubcomponent<GraphMiddlePanel>()->setConfig(
		{
			.scene = config.scene,
			.c_framebuffer = config.viewportFramebuffer,
			.c_minimapFramebuffer = config.minimapFramebuffer,

			.c_viewportPos = config.viewportPos,
			.c_viewportSize = config.viewportSize,
			.startPos = config.selectionWindowStartPos,
			.currPos = config.selectionWindowCurrentPos
		}
	);
	getSubcomponent<GraphMiddlePanel>()->OnImGuiRender();

	//std::vector<std::string> openTabs;
	//for (const auto& [name, _] : managers) {
	//	openTabs.push_back(name);
	//}

	ImGui::TableNextColumn();
	ImGui::BeginChild("Tab 2");


	if (ImGui::BeginTable("RightPanelTable", 2, ImGuiTableFlags_SizingFixedFit))
	{
		float buttonWidth = ImGui::CalcTextSize(">>").x + ImGui::GetStyle().FramePadding.x * 2.0f;
		ImGui::TableSetupColumn("ButtonColumn", ImGuiTableColumnFlags_WidthFixed, buttonWidth);

		if (rightColumnExpanded)
			ImGui::TableSetupColumn("RightPanel", ImGuiTableColumnFlags_WidthStretch);
		else
			ImGui::TableSetupColumn("RightPanel", ImGuiTableColumnFlags_WidthFixed, 0.0f);

		ImGui::TableNextColumn();


		if (ImGui::Button(rightColumnExpanded ? "<<" : ">>"))
			rightColumnExpanded = !rightColumnExpanded;
		ImGui::TableNextColumn();

		getSubcomponent<GraphRightPanel>()->setConfig(
			{
				.scene = config.scene,
				.c_selectedEntities = *config.selectedEntities
			}
		);
		getSubcomponent<GraphRightPanel>()->OnImGuiRender();
		ImGui::EndTable();



	}
	ImGui::EndChild();

	ImGui::EndTable();
	ImGui::End();

	if (DataManager::getInstance().isSaving()) {
		getSubcomponent<MenuDropdownPanel>()->
			getSubcomponent<SavingUI>()->setConfig({
			.c_map = config.map
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

			if (config.scene->setManager(std::string(loadMapPath)))
			{
				auto& world_map(manager->addEntityNoId<Empty>());
				AssetManager::CreateWorldMap(world_map);

				config.map->loadMap(
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

		config.map->loadPaths(loadPathName.c_str(),
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
			.displayedEntity = config.displayedEntity,
				});
		getSubcomponent<GraphMiddlePanel>()->getSubcomponent<ViewportPanel>()->
			getSubcomponent<EntityComponentsControlPanel>()->
			OnImGuiRender();

		getSubcomponent<GraphMiddlePanel>()->getSubcomponent<ViewportPanel>()->
			getSubcomponent<HoverEntityPanel>()->
			setConfig({
			.scene = config.scene,
			.hoveredEntity = config.onHoverEntity,
				});
		getSubcomponent<GraphMiddlePanel>()->getSubcomponent<ViewportPanel>()->
			getSubcomponent<HoverEntityPanel>()->
			OnImGuiRender();
	}

	if (manager && config.sceneManagerActive) {
		getSubcomponent<GraphMiddlePanel>()->getSubcomponent<ViewportPanel>()->
			getSubcomponent<SceneControlPanel>()->
			setConfig({
			.scene = config.scene
				});
		getSubcomponent<GraphMiddlePanel>()->getSubcomponent<ViewportPanel>()->
			getSubcomponent<SceneControlPanel>()->
			OnImGuiRender();
	}
}
