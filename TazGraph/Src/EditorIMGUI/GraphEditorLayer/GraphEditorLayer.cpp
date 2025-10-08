#include "GraphEditorLayer.h"

void GraphEditorLayer::OnImGuiRender()
{
	Manager* manager = config.scene->manager;
	glm::vec2 viewportMousePos = config.viewportMousePosition;

	ImGuiViewport* window = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(window->Pos);
	ImGui::SetNextWindowSize(window->Size);
	if (ImGui::Begin("Main Window", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus))
	{
		getSubcomponent<MenuDropdownPanel>()->setConfig({ .scene = config.scene });
		getSubcomponent<MenuDropdownPanel>()->OnImGuiRender();

		static bool initializedUIColumns = false;


		ImGuiTableFlags flagsTable = ImGuiTableFlags_BordersInnerV;
		if (initializedUIColumns)
			flagsTable |= ImGuiTableFlags_Resizable;

		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		float buttonWidth = ImGui::CalcTextSize(">>").x + ImGui::GetStyle().FramePadding.x * 2.0f;

		// --- Decide widths based on states ---
		float leftWidth, middleWidth, rightWidth;

		leftWidth = viewportSize.x * 0.2f;
		middleWidth = -1.0f; // Stretch column
		rightWidth = viewportSize.x * 0.2f;

		initializedUIColumns = true;

		if (leftColumnState == COL_STATE::COLLAPSED)
		{
			initializedUIColumns = false;
			leftWidth = buttonWidth + 32.0f;
		}
		else if (last_leftColumnState == COL_STATE::COLLAPSED && leftColumnState == COL_STATE::EXPANDED)
		{
			leftWidth = viewportSize.x * 0.25f;
		}

		if (rightColumnState == COL_STATE::COLLAPSED)
		{
			initializedUIColumns = false;
			rightWidth = buttonWidth + 32.0f;
		}
		else if (last_rightColumnState == COL_STATE::COLLAPSED && rightColumnState == COL_STATE::EXPANDED)
		{
			rightWidth = viewportSize.x * 0.25f;
		}

		// --- Build table ---
		if (ImGui::BeginTable("mainColumns", 3, flagsTable))
		{
			ImGui::TableSetupColumn("LeftCol", ImGuiTableColumnFlags_WidthFixed, leftWidth);
			ImGui::TableSetupColumn("MiddleCol", ImGuiTableColumnFlags_WidthStretch, middleWidth);
			ImGui::TableSetupColumn("RightCol", ImGuiTableColumnFlags_WidthFixed, rightWidth);

			ImGui::TableNextColumn();
			if (ImGui::BeginChild("Tab 1"))
			{
				if (ImGui::BeginTable("LeftPanelTable", 2, ImGuiTableFlags_SizingFixedFit))
				{
					if (leftColumnState == COL_STATE::EXPANDED) {
						ImGui::TableSetupColumn("LeftPanel", ImGuiTableColumnFlags_WidthStretch);
					}
					else {
						ImGui::TableSetupColumn("LeftPanel", ImGuiTableColumnFlags_WidthFixed, 0.0f);
					}
					ImGui::TableSetupColumn("ButtonColumn", ImGuiTableColumnFlags_WidthFixed, buttonWidth);
					ImGui::TableNextColumn();

					if (leftColumnState == COL_STATE::EXPANDED)
					{
						getSubcomponent<GraphLeftPanel>()->setConfig({
							.scene = config.scene,
							.sceneMouseCoords = viewportMousePos,
							});
						getSubcomponent<GraphLeftPanel>()->OnImGuiRender();
					}
					ImGui::TableNextColumn();

					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5);
					if (ImGui::Button(leftColumnState == COL_STATE::EXPANDED ? "<<" : ">>", ImVec2(buttonWidth, buttonWidth)))
						leftColumnState = leftColumnState == COL_STATE::EXPANDED ? COL_STATE::COLLAPSED : COL_STATE::EXPANDED;
					ImGui::PopStyleVar();

					ImGui::EndTable();
				}
				ImGui::EndChild();
			}

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


			ImGui::TableNextColumn();

			if (ImGui::BeginChild("Tab 2"))
			{
				if (ImGui::BeginTable("RightPanelTable", 2, ImGuiTableFlags_SizingFixedFit))
				{

					if (rightColumnState == COL_STATE::EXPANDED)
					{
						ImGui::TableSetupColumn("ButtonColumn", ImGuiTableColumnFlags_WidthFixed, buttonWidth);
						ImGui::TableSetupColumn("RightPanel", ImGuiTableColumnFlags_WidthStretch);
					}
					else
					{
						ImGui::TableSetupColumn("ButtonColumn", ImGuiTableColumnFlags_WidthFixed, buttonWidth * 2.0f);
						ImGui::TableSetupColumn("RightPanel", ImGuiTableColumnFlags_WidthFixed, 0.0f);
					}
					ImGui::TableNextColumn();

					if (rightColumnState == COL_STATE::COLLAPSED)
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 16.0f);

					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5);
					if (ImGui::Button(rightColumnState == COL_STATE::EXPANDED ? ">>" : "<<", ImVec2(buttonWidth, buttonWidth)))
						rightColumnState = rightColumnState == COL_STATE::EXPANDED ? COL_STATE::COLLAPSED : COL_STATE::EXPANDED;
					ImGui::TableNextColumn();
					ImGui::PopStyleVar();


					if (rightColumnState) {
						getSubcomponent<GraphRightPanel>()->setConfig(
							{
								.scene = config.scene,
								.c_selectedEntities = *config.selectedEntities
							}
						);
						getSubcomponent<GraphRightPanel>()->OnImGuiRender();
					}

					ImGui::EndTable();
				}

				ImGui::EndChild();
			}

			ImGui::EndTable();
		}
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
					node.GetComponent<Rectangle_w_Color>().color = TazColor(0, 0, 224, 255);

					node.addGroup(Manager::groupNodes_0);

					manager->grid->addNode(&node, manager->grid->getGridLevel());
				}
				for (int i = 0; i < getSubcomponent<MenuDropdownPanel>()->getSubcomponent<NewMapUI>()->newLinksCount; ++i) {
					auto& link = manager->addEntity<Link>(0, i + 1);
					link.addComponent<Line_w_Color>();

					link.GetComponent<Line_w_Color>().setSrcColor(TazColor(255, 40, 0, 255));
					link.GetComponent<Line_w_Color>().setDestColor(TazColor(40, 255, 0, 255));

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

		ImGui::End();
	}
}
