#include "./Graph.h"
#include <AppScene/AppInterface.h>

float nodeRadius = 1.0f;

void Graph::updateUI(float deltaTime) {
	
	_menuDropdown.setConfig({});
	_menuDropdown.update(deltaTime);
	
	_fpsCounter.update(deltaTime);

	_topBar.setConfig(
		{
		.c_fpsLimiter = &getApp()->getFPSLimiter(),
		.c_graphNames = &managers,
		.c_currentActive = &managerName,
		.c_manager = manager,
		}
		);
	_topBar.update(deltaTime);

	_graphLeftPanel.setConfig({
	.renderDebug = &_renderDebug,
	.sceneMouseCoords = _sceneMousePosition,
	.mouseCoords = _app->_inputManager.getMouseCoords(),
	.manager = manager
		});
	_graphLeftPanel.update(deltaTime);
	
	_viewportPanel.setConfig({
	.c_fb = &_framebuffer,
	.c_minimap_fb = &_minimapFramebuffer,
	.c_storedWindowPos = &_windowPos,
	.c_storedWindowSize = &_windowSize
		});
	_viewportPanel.update(deltaTime);
	
	_graphRightPanel.setConfig({
	.c_manager = manager,
	.c_nodeRadius = &nodeRadius,
	.c_selectedEntities = _selectedEntities
		});
	_graphRightPanel.update(deltaTime);
	
	_sceneControl.setConfig(
		{
			.c_mouseCoords = _savedMainViewportMousePosition,
			.c_manager = manager
		}
	);
	_sceneControl.update(deltaTime);

	_entityComponentController.setConfig(
		{
		.mousePos = _savedMainViewportMousePosition, 
		.displayedEntity = _displayedEntity,
		.manager = manager
		}
	);
	_entityComponentController.update(deltaTime);

	_hoverEntityPanel.setConfig
	({
		.mousePos = _app->_inputManager.getMouseCoords(),
		.hoveredEntity = _onHoverEntity,
		.manager = manager
		});
	_hoverEntityPanel.update(deltaTime);
}

void Graph::drawUI() {
	//todo do it like: graphEditorLayer.update(); graphEditorLayer.OnIMGUIRender();
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::Begin("Main Window", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);
	
	_menuDropdown.OnImGuiRender();

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

	_fpsCounter.setLimiter(getApp()->getFPSLimiter());

	_fpsCounter.OnImGuiRender();

	ImGui::BeginChild("Tab 1");

	_graphLeftPanel.OnImGuiRender();

	ImGui::EndChild();

	ImGui::NextColumn();

	//std::vector<std::string> openTabs;
	//for (const auto& [name, _] : managers) {
	//	openTabs.push_back(name);
	//}

	std::string activeManagerKey = managerName;


	_topBar.OnImGuiRender();

	std::string closedTab = _topBar.getTabToClose();
	if (!closedTab.empty()) {
		auto managerIt = managers.find(closedTab);
		if (managerIt != managers.end()) {
			managers.erase(managerIt);

			if (closedTab == activeManagerKey) {
				if (!managerName.empty() &&
					managers.find(managerName) != managers.end()) {
					setManager(managerName);
				}
				else if (!managers.empty()) {
					setManager(managers.begin()->first);
				}
				else {
					activeManagerKey = "";
				}
			}
		}
	}
	else if (activeManagerKey != managerName && !managerName.empty()) {
		// Normal tab switching (no closure)
		auto managerIt = managers.find(managerName);
		if (managerIt != managers.end()) {
			setManager(managerName);
		}
	}


	_viewportPanel.OnImGuiRender();

	ImGui::NextColumn();
	ImGui::BeginChild("Tab 2");


	_graphRightPanel.OnImGuiRender();
	
	ImGui::EndChild();

	ImGui::End();

	if (DataManager::getInstance().isSaving()) {
		_menuDropdown.savingUI.setConfig({
			.c_map = map
			});
		_menuDropdown.savingUI.OnImGuiRender();
	}
	if (DataManager::getInstance().isStartingNew()) {
		_menuDropdown.newMapUI.OnImGuiRender();

		if (!DataManager::getInstance().isStartingNew()) {
			std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

			float spacing = 120.0f; // Space between nodes

			float totalWidth = (_menuDropdown.newMapUI.newNodesCount - 1) * spacing;
			float startX = -totalWidth * 0.5f;
			float y = 0.0f;

			for (int i = 0; i < _menuDropdown.newMapUI.newNodesCount; ++i) {
				auto& node = manager->addEntity<Node>();
				glm::vec2 position = glm::vec2(startX + i * spacing, y);
				node.addComponent<TransformComponent>(position, Layer::action, glm::vec3(10.0f), 1);
				node.addComponent<Rectangle_w_Color>();
				node.GetComponent<Rectangle_w_Color>().color = Color(0, 0, 224, 255);

				node.addGroup(Manager::groupNodes_0);

				manager->grid->addNode(&node, manager->grid->getGridLevel());
			}
			for (int i = 0; i < _menuDropdown.newMapUI.newLinksCount; ++i) {
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
		_menuDropdown.loadingUI.setConfig({});
		_menuDropdown.loadingUI.OnImGuiRender();
		char* loadMapPath = DataManager::getInstance().data.input;
		if (strlen(loadMapPath) && !DataManager::getInstance().isLoading()) {

			if (setManager(std::string(loadMapPath)))
			{
				auto& world_map(manager->addEntityNoId<Empty>());
				AssetManager::CreateWorldMap(world_map);

				map->loadMap(
					loadMapPath,
					std::bind(&Map::AddDefaultNode, map, std::placeholders::_1, std::placeholders::_2),
					std::bind(&Map::AddDefaultLink, map, std::placeholders::_1),
					&_app->threadPool
				);
			}
		}
	}
	if (DataManager::getInstance().isLoadingPath()) {
		DataManager::getInstance().setPathLoading(false);

		std::string loadPathName = DataManager::getInstance().getPathLoading();

		map->loadPaths(loadPathName.c_str(),
			std::bind(&Map::AddDefaultNode, map, std::placeholders::_1, std::placeholders::_2),
			std::bind(&Map::AddPathLink, map, std::placeholders::_1),
			nullptr);
		std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

		main_camera2D->makeCameraDirty();
		manager->aboutTo_updateActiveEntities();
	}
	if (DataManager::getInstance().isGoingBack()) {
		_currentState = SceneState::CHANGE_PREVIOUS;
		DataManager::getInstance().SetGoingBack(false);
	}


	//glm::vec2 worldToVieport
	if (manager) {
		_entityComponentController.OnImGuiRender();
		_hoverEntityPanel.OnImGuiRender();
	}

	if (manager && _sceneManagerActive) {

		_sceneControl.OnImGuiRender();
	}
	// this is going to be shown when right click on scene and no displayEntity shows


}
