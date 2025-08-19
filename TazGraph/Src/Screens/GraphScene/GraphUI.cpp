#include "./Graph.h"
#include "../../AssetManager/AssetManager.h"
#include <AppScene/AppInterface.h>

float nodeRadius = 1.0f;

void Graph::updateUI() {
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::Begin("Main Window", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);
	_editorImgui.MenuBar();

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

	_editorImgui.FPSCounter(getApp()->getFPSLimiter());
	ImGui::BeginChild("Tab 1");

	_editorImgui.LeftColumnUIElement(_renderDebug, _clusterLayout, _sceneMousePosition, _app->_inputManager.getMouseCoords(), *manager, _onHoverEntity, _backgroundColor, CELL_SIZE);

	ImGui::EndChild();

	ImGui::NextColumn();

	std::vector<std::string> openTabs;
	for (const auto& [name, _] : managers) {
		openTabs.push_back(name);
	}

	std::string activeManagerKey = managerName;

	std::string selectedTab = _editorImgui.SceneTabs(openTabs, activeManagerKey);
	if (selectedTab != managerName) {
		setManager(selectedTab);
	}

	_editorImgui.updateIsMouseInSecondColumn();

	_editorImgui.SceneViewport(
		getApp()->getFPSLimiter(),
		*manager,
		_framebuffer,
		_minimapFramebuffer,
		_windowPos, _windowSize);

	ImGui::NextColumn();
	ImGui::BeginChild("Tab 2");


	_editorImgui.RightColumnUIElement(*manager, &nodeRadius);


	ImGui::EndChild();

	ImGui::End();


	_editorImgui.scriptResultsVisualization(*manager, _selectedEntities);

	if (_editorImgui.isSaving()) {
		_editorImgui.SavingUI(map);
	}
	if (_editorImgui.isStartingNew()) {
		_editorImgui.NewMapUI();

		if (!_editorImgui.isStartingNew()) {
			std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

			float spacing = 120.0f; // Space between nodes

			float totalWidth = (_editorImgui.newNodesCount - 1) * spacing;
			float startX = -totalWidth * 0.5f;
			float y = 0.0f;

			for (int i = 0; i < _editorImgui.newNodesCount; ++i) {
				auto& node = manager->addEntity<Node>();
				glm::vec2 position = glm::vec2(startX + i * spacing, y);
				node.addComponent<TransformComponent>(position, Layer::action, glm::vec3(10.0f), 1);
				node.addComponent<Rectangle_w_Color>();
				node.GetComponent<Rectangle_w_Color>().color = Color(0, 0, 224, 255);

				node.addGroup(Manager::groupNodes_0);

				manager->grid->addNode(&node, manager->grid->getGridLevel());
			}
			for (int i = 0; i < _editorImgui.newLinksCount; ++i) {
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
	if (_editorImgui.isLoading()) {
		char* loadMapPath = _editorImgui.LoadingUI();
		if (!_editorImgui.isLoading()) {

			if (setManager(std::string(loadMapPath)))
			{
				auto& world_map(manager->addEntityNoId<Empty>());
				_assetsManager->CreateWorldMap(world_map);

				map->loadMap(
					loadMapPath,
					std::bind(&Map::AddDefaultNode, map, std::placeholders::_1, std::placeholders::_2),
					std::bind(&Map::AddDefaultLink, map, std::placeholders::_1),
					&_app->threadPool
				);
			}
		}
	}
	if (_editorImgui.isLoadingPath()) {
		_editorImgui.setPathLoading(false);

		std::string loadPathName = _editorImgui.getPathLoading();

		map->loadPaths(loadPathName.c_str(),
			std::bind(&Map::AddDefaultNode, map, std::placeholders::_1, std::placeholders::_2),
			std::bind(&Map::AddPathLink, map, std::placeholders::_1),
			nullptr);
		std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

		main_camera2D->makeCameraDirty();
		manager->aboutTo_updateActiveEntities();
	}
	if (_editorImgui.isGoingBack()) {
		_currentState = SceneState::CHANGE_PREVIOUS;
		_editorImgui.SetGoingBack(false);
	}


	//glm::vec2 worldToVieport
	if (manager) {
		_editorImgui.ShowEntityComponents(_savedMainViewportMousePosition, _displayedEntity, *manager);
	}


	if (manager && _sceneManagerActive) {
		_editorImgui.ShowSceneControl(_savedMainViewportMousePosition, *manager);
	}
	// this is going to be shown when right click on scene and no displayEntity shows


	glClearColor(_backgroundColor[0], _backgroundColor[1], _backgroundColor[2], _backgroundColor[3]);
}
