#include "EditorIMGUI.h"
#include "GECS/Core/GECSUtil.h"

EditorIMGUI::EditorIMGUI() {
	ReloadAccessibleFiles();
}

EditorIMGUI::~EditorIMGUI() {

}

bool EditorIMGUI::isSaving() {
	return _isSaving;
}

void EditorIMGUI::setNewMap(bool startingNew)
{
	_isStartingNew = startingNew;
}

void EditorIMGUI::setLoading(bool loading)
{
	_isLoading = loading;
}

bool EditorIMGUI::isStartingNew()
{
	return _isStartingNew;
}

bool EditorIMGUI::isLoading()
{
	return _isLoading;
}

bool EditorIMGUI::isGoingBack()
{
	return _goingBack;
}

void EditorIMGUI::SetGoingBack(bool goingBack) {
	_goingBack = goingBack;
}

void EditorIMGUI::updateFileNamesInAssets() {
	_fileNames.clear();
	const std::string path = "assets/Maps"; // Directory path
	for (const auto& entry : fs::directory_iterator(path)) {
		if (entry.is_regular_file()) {
			_fileNames.push_back(entry.path().filename().string()); // Add file name to vector
		}
	}
}

void EditorIMGUI::updatePollingFileNamesInAssets() {
	_pollingFileNames.clear();
	const std::string path = "assets/Maps/Polling"; // Directory path
	for (const auto& entry : fs::directory_iterator(path)) {
		if (entry.is_regular_file()) {
			_pollingFileNames.push_back(entry.path().filename().string()); // Add file name to vector
		}
	}
}

void EditorIMGUI::ReloadAccessibleFiles() {
	if (!_filesLoaded) {
		updateFileNamesInAssets();
		updatePollingFileNamesInAssets();

		_filesLoaded = true; // Set to true so we don't reload unnecessarily
	}
}

bool* EditorIMGUI::getDockspaceRef()
{
	return &_dockingEnabled;
}

void EditorIMGUI::MenuBar() {
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save")) {
				_isSaving = true;
			}
			if (ImGui::MenuItem("Load")) {
				_isLoading = true;
			}
			if (ImGui::MenuItem("Back")) {
				_goingBack = true;
			}
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

}

bool EditorIMGUI::isMouseOnWidget(const std::string& widgetName)
{
	ImGuiContext& g = *ImGui::GetCurrentContext(); // Get ImGui context

	for (ImGuiWindow* window : g.Windows) {
		if (window->Name == widgetName && !(window->Hidden || (window->Active == false))) {
			ImVec2 widgetPos = window->Pos;
			ImVec2 widgetSize = window->Size;

			ImVec2 mousePos = ImGui::GetMousePos();
			
			return (mousePos.x >= widgetPos.x && mousePos.x <= (widgetPos.x + widgetSize.x) &&
				mousePos.y >= widgetPos.y && mousePos.y <= (widgetPos.y + widgetSize.y));
		}
	}

	return false;
}

void EditorIMGUI::LeftColumnUIElement(bool &renderDebug, bool &clusterLayout, glm::vec2 mouseCoords, glm::vec2 mouseCoords2, Manager& manager, Entity* onHoverEntity, float(& backgroundColor)[4], int cell_size) {
	ImGui::BeginChild("Background UI");
	ImGui::ColorEdit4("Background Color", backgroundColor);
	
	ImVec4 activeColor = ImVec4(0.2f, 0.7f, 0.2f, 1.0f);
	ImVec4 inactiveColor = ImVec4(0.7f, 0.2f, 0.2f, 1.0f);

	ImVec4 defaultColor = ImVec4(0.0f, 0.5f, 1.0f, 1.0f);
	


	ImGui::Separator();

	ImGui::Text("Camera:");

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	ImGui::Text("Rect: {x: %f, y: %f, w: %f, h: %f}", main_camera2D->getCameraRect().x, main_camera2D->getCameraRect().y, main_camera2D->getCameraRect().w, main_camera2D->getCameraRect().h);

	if (ImGui::SliderFloat3("Eye Position", &main_camera2D->eyePos[0], -1000.0f, 1000.0f)) {
		main_camera2D->setCameraMatrix(glm::lookAt(main_camera2D->eyePos, main_camera2D->aimPos, main_camera2D->upDir));
	}
	if (ImGui::SliderFloat3("Aim Position", &main_camera2D->aimPos[0], -1000.0f, 1000.0f)) {
		main_camera2D->setCameraMatrix(glm::lookAt(main_camera2D->eyePos, main_camera2D->aimPos, main_camera2D->upDir));
	}
	if (ImGui::SliderFloat3("Up Direction", &main_camera2D->upDir[0], -1000.0f, 1000.0f)) {
		main_camera2D->setCameraMatrix(glm::lookAt(main_camera2D->eyePos, main_camera2D->aimPos, main_camera2D->upDir));
	}

	if (ImGui::SliderFloat("Rotate Around Z", &_cameraRotationZ, 0.0f, 360.0f)) {
		float angleRad = glm::radians(_cameraRotationZ);
		float radius = 3000.0f;

		// Calculate new eye position in the XY plane (Z remains the same)
		float x = cos(angleRad) * radius;
		float y = sin(angleRad) * radius;
		float z = main_camera2D->getPosition().z; // Maintain original height

		glm::vec3 newEyePos = glm::vec3(x, y, z);
		main_camera2D->setPosition(newEyePos);

		// Always aim at origin or fixed point (optional)
		main_camera2D->setAimPos(glm::vec3(0.0f, 0.0f, 0.0f));
	}

	const char* viewModeNames[] = { "Y-Up", "Z-Up" };

	if (ImGui::Combo("Orientation", &_currentOrientationIndex, viewModeNames, IM_ARRAYSIZE(viewModeNames))) {
		ViewMode newMode = static_cast<ViewMode>(_currentOrientationIndex);
		main_camera2D->currentViewMode = newMode;

		glm::vec3 eyePos = main_camera2D->getPosition();

		if (newMode == ViewMode::Y_UP) {
			main_camera2D->upDir = glm::vec3(0, -1, 0);
			main_camera2D->setAimPos(glm::vec3(eyePos.x, eyePos.y, eyePos.z + 1.0f));
		}
		else if (newMode == ViewMode::Z_UP) {
			main_camera2D->upDir = glm::vec3(0, 0, -1);
			main_camera2D->setAimPos(glm::vec3(eyePos.x, eyePos.y + 1.0f, eyePos.z));
		}

	}

	if (ImGui::Button("Reset")) {
		main_camera2D->resetCameraPosition(); 
	}

	ImGui::Separator();
	// Change color based on the debug mode state
	if (renderDebug) {
		ImGui::PushStyleColor(ImGuiCol_Button, activeColor);  // Green for ON
	}
	else {
		ImGui::PushStyleColor(ImGuiCol_Button, inactiveColor);  // Red for OFF
	}
	// Button toggles the debug mode
	if (ImGui::Button("Enable Debug Mode")) {
		renderDebug = !renderDebug;  // Toggle the state
	}

	ImGui::PopStyleColor(1);
	
	ImGui::Separator();

	if (ImGui::Button(manager.arrowheadsEnabled ? "Disable Arrowheads" : "Enable Arrowheads")) {
		manager.arrowheadsEnabled = !manager.arrowheadsEnabled;
		//manager.setArrowheadsEnabled(arrowheadsEnabled); // Call function to apply change
	}
	
	ImGui::Separator();

	ImGui::Text("Select Grouping Layout:");
	ImGui::PushStyleColor(ImGuiCol_Button, activeLayout == 0 ? activeColor : defaultColor);
	if (ImGui::Button("Default Layout", ImVec2(120, 30))) {
		activeLayout = 0;
	}
	ImGui::PopStyleColor();

	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Button, activeLayout == 1 ? activeColor : defaultColor);
	if (ImGui::Button("Group Layout 1", ImVec2(120, 30))) {
		activeLayout = 1;
	}
	ImGui::PopStyleColor();

	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Button, activeLayout == 2 ? activeColor : defaultColor);
	if (ImGui::Button("Group Layout 2", ImVec2(120, 30))) {
		activeLayout = 2;
	}
	ImGui::PopStyleColor();

	ImGui::Separator();

	ImGui::Text("Choose Layout:");

	if (ImGui::Button("Circular", ImVec2(120, 30))) {
		
		auto& nodes = manager.getGroup<NodeEntity>(Manager::groupNodes_0);
		if (nodes.empty()) return;

		NodeEntity* centerNode = nullptr;
		int maxOutlinks = -1;

		for (NodeEntity* node : nodes) {
			int outLinks = node->getOutLinks().size();
			if (outLinks > maxOutlinks) {
				maxOutlinks = outLinks;
				centerNode = node;
			}
		}

		if (!centerNode) return;

		glm::vec2 centerPos = glm::vec2(0.0f, 0.0f);
		centerNode->GetComponent<TransformComponent>().setPosition_X(centerPos.x);
		centerNode->GetComponent<TransformComponent>().setPosition_Y(centerPos.y);

		float minRadius = 100.0f;
		float maxRadius = 3000.0f;

		int minOutlinks = INT_MAX;
		for (NodeEntity* node : nodes) {
			if (node == centerNode) continue;
			int count = node->getOutLinks().size();
#if defined(_WIN32) || defined(_WIN64)
			minOutlinks = min(minOutlinks, count);
			maxOutlinks = max(maxOutlinks, count);
#else
			minOutlinks = std::min(minOutlinks, count);
			maxOutlinks = std::max(maxOutlinks, count);
#endif

		}


		size_t index = 0;
		size_t total = nodes.size() - 1;

		for (NodeEntity* node : nodes) {
			if (node == centerNode) continue;

			float angle = (2 * M_PI * index) / total;
			int outLinks = node->getOutLinks().size();

#if defined(_WIN32) || defined(_WIN64)
			float normalized = (float)(maxOutlinks - outLinks) / max(1, maxOutlinks - minOutlinks);
#else
			float normalized = (float)(maxOutlinks - outLinks) / std::max(1, maxOutlinks - minOutlinks);
#endif
			float radius = minRadius + normalized * (maxRadius - minRadius);

			glm::vec2 pos = centerPos + glm::vec2(cos(angle), sin(angle)) * radius;

			node->GetComponent<TransformComponent>().setPosition_X(pos.x);
			node->GetComponent<TransformComponent>().setPosition_Y(pos.y);


			++index;
		}

		manager.aboutTo_updateActiveEntities();

		for (auto& n : manager.getGroup<NodeEntity>(Manager::groupNodes_0))
		{
			n->cellUpdate();
		}
		for (auto& n : manager.getGroup<NodeEntity>(Manager::groupGroupNodes_0))
		{
			n->cellUpdate();
		}
		for (auto& n : manager.getGroup<NodeEntity>(Manager::groupGroupNodes_1))
		{
			n->cellUpdate();
		}
		for (auto& l : manager.getGroup<LinkEntity>(Manager::groupLinks_0))
		{
			l->cellUpdate();
		}
		for (auto& l : manager.getGroup<LinkEntity>(Manager::groupGroupLinks_0))
		{
			l->cellUpdate();
		}
		for (auto& l : manager.getGroup<LinkEntity>(Manager::groupGroupLinks_1))
		{
			l->cellUpdate();
		}
	}

	if (clusterLayout) {
		ImGui::PushStyleColor(ImGuiCol_Button, activeColor);  // Green for ON
	}
	else {
		ImGui::PushStyleColor(ImGuiCol_Button, inactiveColor);  // Red for OFF
	}

	if (ImGui::Button(clusterLayout ? "Disable Cluster" : "Cluster", ImVec2(120, 30))) {
		
		auto clusterGroupLayout = [&](Group nodeGroup, Group linkGroup)
			{
				auto& nodes = manager.getGroup<NodeEntity>(nodeGroup);
				auto& links = manager.getGroup<LinkEntity>(linkGroup);

				if (clusterLayout) {
					/*for (NodeEntity* node : nodes) {
						node->addGroup(Manager::groupColliders);
					}*/

					for (NodeEntity* node : nodes) {
						node->addGroup(Manager::groupColliders);
						node->addComponent<ColliderComponent>(
							&manager,
							node->GetComponent<TransformComponent>().size);

						node->GetComponent<ColliderComponent>().addCollisionGroup(nodeGroup);
					}

					for (LinkEntity* link : links) {
						link->addComponent<SpringComponent>();
					}
				}
				else {
					for (NodeEntity* node : nodes) {
						if (node->hasComponent<ColliderComponent>()) {
							node->removeGroup(Manager::groupColliders);
							node->removeComponent<ColliderComponent>();
						}
					}
					for (LinkEntity* link : links) {
						if (link->hasComponent<SpringComponent>()) {
							link->removeComponent<SpringComponent>();
						}
					}
				}
			};

		clusterLayout = !clusterLayout;
		
		clusterGroupLayout(Manager::groupNodes_0, Manager::groupLinks_0);
		clusterGroupLayout(Manager::groupGroupNodes_0, Manager::groupGroupLinks_0);
		clusterGroupLayout(Manager::groupGroupNodes_1, Manager::groupGroupLinks_1);

		manager.aboutTo_updateActiveEntities();

	}
	ImGui::PopStyleColor(1);

	ImGui::Separator();



	if (ImGui::BeginTable("GroupsTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
		ImGui::TableSetupColumn("Group Name", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Entity Count", ImGuiTableColumnFlags_WidthFixed, 100.0f);
		ImGui::TableHeadersRow();

		int totalEntities = 0;
		for (std::size_t managerGroup = Manager::groupBackgroundLayer; managerGroup != Manager::buttonLabels + 1; ++managerGroup) {
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("%s", manager.getGroupName(managerGroup).c_str());
			ImGui::TableSetColumnIndex(1);
			int groupSize = manager.getVisibleGroup<EmptyEntity>(managerGroup).size() + manager.getVisibleGroup<NodeEntity>(managerGroup).size() + manager.getVisibleGroup<LinkEntity>(managerGroup).size();
			ImGui::Text("%d", groupSize);

			totalEntities += groupSize;
		}

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Total Visible Entities");
		ImGui::TableSetColumnIndex(1);
		ImGui::Text("%d", totalEntities);

		ImGui::EndTable();
	}

	if (ImGui::BeginTable("GroupsTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
		ImGui::TableSetupColumn("Vectors", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Size()", ImGuiTableColumnFlags_WidthFixed, 100.0f);
		ImGui::TableHeadersRow();

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("entities");
		ImGui::TableSetColumnIndex(1);
		ImGui::Text("%zu", manager.getEntities().size());

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("visible empty entities");
		ImGui::TableSetColumnIndex(1);
		ImGui::Text("%zu", manager.getVisible<EmptyEntity>().size());

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("visible nodes");
		ImGui::TableSetColumnIndex(1);
		ImGui::Text("%zu", manager.getVisible<NodeEntity>().size());

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("visible links");
		ImGui::TableSetColumnIndex(1);
		ImGui::Text("%zu", manager.getVisible<LinkEntity>().size());
		
		ImGui::EndTable();
	}

	ImGui::Text("Grid Size: %u", manager.grid->getCellSize());


	ImGui::Separator();


	ImGui::Text("Scene/Screen Coords: {x: %f, y: %f}", mouseCoords.x, mouseCoords.y);
	ImGui::Text("MainViewport Coords: {x: %f, y: %f}", mouseCoords2.x, mouseCoords2.y);


	if (onHoverEntity) {
		ImGui::Text("Selected Entity Details");

		Node* node = dynamic_cast<Node*>(onHoverEntity);
		if (node) {

			ImGui::Text("Id: %d", onHoverEntity->getId());

			TransformComponent* tr = &onHoverEntity->GetComponent<TransformComponent>();
			ImGui::Text("Position: (%f, %f)", tr->getPosition().x, tr->getPosition().y);
			ImGui::Text("Size: (%f, %f)", tr->size.x, tr->size.y);
			glm::vec3 cellBox = manager.grid->getCell(*onHoverEntity, manager.grid->getGridLevel())->boundingBox_origin;
			ImGui::Text("Grid x: %.2f and y: %.2f", cellBox.x, cellBox.y);
		}
		Link* link = dynamic_cast<Link*>(onHoverEntity);
		if (link) {
			ImGui::Text("Id: %d", onHoverEntity->getId());

		}

	}

	ImGui::EndChild();
}

void EditorIMGUI::RightColumnUIElement(Manager& manager, float* nodeRadius) {
	availableFunctions();
	ShowFunctionExecutionResults();
	ImGui::Separator();
	ShowAllEntities(manager, *nodeRadius);
}

void EditorIMGUI::FPSCounter(const BaseFPSLimiter& baseFPSLimiter) {

	ImGui::Begin("Performance");
	ImGui::Text("FPS: %f", baseFPSLimiter.fps);
	if (ImPlot::BeginPlot("FPS Plot")) {
#if defined(_WIN32) || defined(_WIN64)
		int plot_count = min(baseFPSLimiter.fps_history_count,
			baseFPSLimiter.fpsHistoryIndx); // Ensuring we do not read out of bounds
		int plot_offset = max(0,
			baseFPSLimiter.fpsHistoryIndx - baseFPSLimiter.fps_history_count); // Ensure a positive offset
#else
		int plot_count = std::min(baseFPSLimiter.fps_history_count,
			baseFPSLimiter.fpsHistoryIndx); // Ensuring we do not read out of bounds
		int plot_offset = std::max(0,
			baseFPSLimiter.fpsHistoryIndx - baseFPSLimiter.fps_history_count); // Ensure a positive offset

#endif

		ImPlot::SetupAxesLimits(0, 100, 0, 200);

		ImPlot::PlotLine("FPS", &baseFPSLimiter.fpsHistory[0], plot_count);

		ImPlot::EndPlot();
	}
	ImGui::End();

}



void EditorIMGUI::SavingUI(Map* map) {

	ImGuiIO& io = ImGui::GetIO();
	ImVec2 windowSize(400, 100); // Desired window size
	ImVec2 windowPos((io.DisplaySize.x - windowSize.x) * 0.5f,
		(io.DisplaySize.y - windowSize.y) * 0.5f);

	ImGui::SetNextWindowPos(windowPos);
	ImGui::SetNextWindowSize(windowSize);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;
	bool open = true;
	ImGui::Begin("Saving...", &open, window_flags);

	ReloadAccessibleFiles();

	_data.SetSelectData(std::move(_fileNames));

	if (ImGui::ComboAutoSelect("Select File", _data)) {
	}
	ImGui::NewLine();
	float windowWidth = ImGui::GetContentRegionAvail().x;
	float buttonWidth = 100; // Define the button width you want
	ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f); // Center the button

	if (ImGui::Button("Save", ImVec2(buttonWidth, 0))) {
		map->saveMapAsText(_data.input); // save Map that is selected
		_isSaving = false;
		_filesLoaded = false;
	}

	if (!open) {
		_isSaving = false;
	}

	ImGui::End();
}


void EditorIMGUI::NewMapUI() {
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 windowSize(400, 100); // Desired window size
	ImVec2 windowPos((io.DisplaySize.x - windowSize.x) * 0.5f,
		(io.DisplaySize.y - windowSize.y) * 0.5f);

	ImGui::SetNextWindowPos(windowPos);
	ImGui::SetNextWindowSize(windowSize);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;
	bool open = true;
	ImGui::Begin("New Map...", &open, window_flags);

	float windowWidth = ImGui::GetContentRegionAvail().x;
	float buttonWidth = 100; // Define the button width you want
	ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f); // Center the button

	ImGui::InputInt("Number of Nodes", &newNodesCount);
	ImGui::InputInt("Number of Links", &newLinksCount);

	if (newNodesCount < 0) {
		newNodesCount = 0;
	}
	if (newLinksCount < 0) {
		newLinksCount = 0;
	}

	if (newNodesCount != 0 && newLinksCount > newNodesCount - 1) {
		newLinksCount = newNodesCount - 1;
	}


	if (ImGui::Button("Start", ImVec2(buttonWidth, 0))) {
		_isStartingNew = false;
	}

	if (!open) {
		_isStartingNew = false;
	}

	ImGui::End();

	return;
}

char* EditorIMGUI::LoadingUI() {
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 windowSize(400, 100); // Desired window size
	ImVec2 windowPos((io.DisplaySize.x - windowSize.x) * 0.5f,
		(io.DisplaySize.y - windowSize.y) * 0.5f);

	ImGui::SetNextWindowPos(windowPos);
	ImGui::SetNextWindowSize(windowSize);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;
	bool open = true;
	ImGui::Begin("Loading...", &open, window_flags);

	ReloadAccessibleFiles();

	_data.SetSelectData(std::move(_fileNames));

	if (ImGui::ComboAutoSelect("Select File", _data)) {
	}
	ImGui::NewLine();
	float windowWidth = ImGui::GetContentRegionAvail().x;
	float buttonWidth = 100; // Define the button width you want
	ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f); // Center the button

	if (ImGui::Button("Load", ImVec2(buttonWidth, 0))) {
		_isLoading = false;
	}

	if (!open) {
		std::memset(_data.input, 0, sizeof(_data.input));
		_isLoading = false;
	}

	ImGui::End();
	
	return _data.input;
}

void EditorIMGUI::MainMenuUI(std::function<void()> onStartSimulator, std::function<void()> onLoadSimulator, std::function<void()> onExitSimulator)
{
	float windowWidth = 200; // Increased window width
	float windowHeight = 200;
	float buttonWidth = 180;  // Define a larger button width
	float buttonHeight = 50;  // Define a larger button height

	ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));  // Set the size of the window as needed
	ImGui::Begin("Control Window", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse);

	float buttonPosX = (windowWidth - buttonWidth) * 0.5f; // Center the button
	float buttonSpacing = 5;

	ImGui::SetCursorPosX(buttonPosX);
	if (ImGui::Button("Start New", ImVec2(buttonWidth, buttonHeight))) {
		onStartSimulator();
	}

	ImGui::Dummy(ImVec2(0.0f, buttonSpacing));

	ImGui::SetCursorPosX(buttonPosX);
	if (ImGui::Button("Load", ImVec2(buttonWidth, buttonHeight))) {
		onLoadSimulator();
	}

	ImGui::Dummy(ImVec2(0.0f, buttonSpacing));

	ImGui::SetCursorPosX(buttonPosX);
	if (ImGui::Button("Exit", ImVec2(buttonWidth, buttonHeight))) {
		onExitSimulator();
	}

	ImGui::End();
}

void EditorIMGUI::ShowAllEntities(Manager& manager, float &m_nodeRadius) {

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));



	ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	float size = 10;

	for (std::size_t group = Manager::groupBackgroundLayer; group != Manager::buttonLabels + 1; group++) {
		std::string s = manager.getGroupName(group);

		if (ImGui::CollapsingHeader(s.c_str())) {
			std::vector<NodeEntity*>& groupVec = manager.getGroup<NodeEntity>(group);

			if ( group == Manager::groupNodes_0 || group == Manager::groupGroupNodes_0 || group == Manager::groupGroupNodes_1) {

				for (auto& entity : groupVec) { // loops 1 time
					Color initialColor = entity->GetComponent<Rectangle_w_Color>().color;
					color = ImVec4(initialColor.r / 255.0f, initialColor.g / 255.0f, initialColor.b / 255.0f, initialColor.a / 255.0f);

					TransformComponent* tr = &entity->GetComponent<TransformComponent>();
					float initialSize[2] = { tr->size.x, tr->size.y };
					size = initialSize[0];
					break;
				}

				if (ImGui::ColorEdit4(("Color##" + s).c_str(), (float*)&color)) {
					std::vector<NodeEntity*>& groupVec = manager.getGroup<NodeEntity>(group);
					Color newColor = {
					   (GLubyte)(color.x * 255),
					   (GLubyte)(color.y * 255),
					   (GLubyte)(color.z * 255),
					   (GLubyte)(color.w * 255)
					};

					for (auto& entity : groupVec) {
						entity->GetComponent<Rectangle_w_Color>().color = newColor;
					}
				}

				if (ImGui::SliderFloat("Node Size", &size, 0, 100)) {
					for (auto& entity : groupVec) {
							entity->GetComponent<TransformComponent>().size.x = size;
							entity->GetComponent<TransformComponent>().size.y = size;
					}
				}

				ImGui::SliderFloat("Border Radius", &m_nodeRadius, 0, 1.0f);

			}

			if (group == Manager::groupLinks_0 || group == Manager::groupGroupLinks_0 || group == Manager::groupGroupLinks_1) {
				for (auto& entity : groupVec) { // loops 1 time
					Color initialColor = entity->GetComponent<Line_w_Color>().src_color;
					color = ImVec4(initialColor.r / 255.0f, initialColor.g / 255.0f, initialColor.b / 255.0f, initialColor.a / 255.0f);

					break;
				}

				if (ImGui::ColorEdit4(("Color##" + s).c_str(), (float*)&color)) {
					std::vector<LinkEntity*>& groupVec = manager.getGroup<LinkEntity>(group);
					Color newColor = {
					   (GLubyte)(color.x * 255),
					   (GLubyte)(color.y * 255),
					   (GLubyte)(color.z * 255),
					   (GLubyte)(color.w * 255)
					};

					for (auto& entity : groupVec) {
						entity->GetComponent<Line_w_Color>().src_color = newColor;
						entity->GetComponent<Line_w_Color>().dest_color = newColor;
					}
				}

				/*if (ImGui::SliderInt("Line Width", &size, 0, 100)) {
					for (auto& entity : groupVec) {
						entity->GetComponent<TransformComponent>().width = size;
						entity->GetComponent<TransformComponent>().height = size;
					}
				}*/
			}

			
			std::string treeNodeLabel = "Entities##" + manager.getGroupName(group);
			if (ImGui::TreeNode(treeNodeLabel.c_str())) {

				for (auto& entity : groupVec) {

					std::string label = "Entity ID: " + std::to_string(entity->getId());

					if (ImGui::TreeNode(label.c_str())) {

						// Create a unique button label
						std::string buttonLabel = "Go to##" + std::to_string(entity->getId());
						if (ImGui::Button(buttonLabel.c_str())) {
							if (entity->hasComponent<TransformComponent>()) {
								main_camera2D->setPosition_X(entity->GetComponent<TransformComponent>().getPosition().x);
								main_camera2D->setPosition_Y(entity->GetComponent<TransformComponent>().getPosition().y);

								main_camera2D->setAimPos(glm::vec3(main_camera2D->eyePos.x, main_camera2D->eyePos.y, main_camera2D->eyePos.z + 1.0f));
							}
						}

						ImGui::TreePop();
					}

				}
				ImGui::TreePop();
			}
			
		}
	}

}

void EditorIMGUI::availableFunctions() {

	if (ImGui::Button("Calculate Degree Of Selected Entities")) {
		_customFunctions.activatedScriptShown = 1;
	}

	if (ImGui::Button("Show Script Results")) {
		_customFunctions.isScriptResultsOpen = !_customFunctions.isScriptResultsOpen;
	}

}

void EditorIMGUI::SceneViewport(uint32_t textureId, ImVec2& storedWindowPos, ImVec2& storedWindowSize) {
	ImGui::BeginChild("Viewport");
	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	//ImGui::Image();
	ImGui::Image(
		reinterpret_cast<void*>(static_cast<uintptr_t>(textureId)),
		viewportPanelSize,
		ImVec2(0, 1),
		ImVec2(1, 0)
	);

	storedWindowPos = ImGui::GetWindowPos(); // You need to call this when the window is in context (i.e., between ImGui::Begin and ImGui::End)
	storedWindowSize = viewportPanelSize;

	ImGui::EndChild();
}

void EditorIMGUI::scriptResultsVisualization(Manager& manager, std::vector<std::pair<Entity*, glm::vec3>>& m_selectedEntities) {

	_customFunctions.renderUI(manager, m_selectedEntities);
}

std::string EditorIMGUI::SceneTabs(const std::vector<std::string>& graphNames, std::string& currentActive) {
	float childHeight = 30.0f;
	
	ImGui::BeginChild("Scene Tabs", ImVec2(0, childHeight), true, ImGuiWindowFlags_NoScrollbar);
	

	if (ImGui::BeginTabBar("SceneTabs", ImGuiTabBarFlags_AutoSelectNewTabs)) {
		for (size_t i = 0; i < graphNames.size(); ++i) {
			const std::string& name = graphNames[i];

			bool open = true;

			if (ImGui::BeginTabItem(name.c_str(), &open, ImGuiTabItemFlags_None)) {
				currentActive = name;
				ImGui::EndTabItem();
			}
			if (!open) {
				// handle tab close if needed (e.g. mark for deletion)
			}
		}
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - 16);
		if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(TextureManager::getInstance().Get_GLTexture("play-button")->id)), ImVec2(16, 16))) {
			interpolation_running = !interpolation_running;
		}

		ImGui::EndTabBar();
	}
	ImGui::EndChild();
	ImGui::BeginChild("Interpolation Slider", ImVec2(0, 40), true);
	{
		ImGui::Text("Interpolation");
		ImGui::SameLine();
		ImGui::SliderFloat("##interp", &interpolation, 0.0f, 1.0f, "%.2f");

		// Optional: Add tooltip
		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::Text("Control interpolation");
			ImGui::EndTooltip();
		}
		ImGui::SameLine();
		ImGui::Text("Speed");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100.0f);
		ImGui::SliderFloat("##interp_speed", &interpolation_speed, 0.01f, 1.0f, "%.2f");
	}

	ImGui::EndChild();


	return currentActive;
}

void EditorIMGUI::ShowFunctionExecutionResults() {
	ImGui::Text("Function Execution Results");
}

void EditorIMGUI::updateIsMouseInSecondColumn() {
	// cant check with the checkIfMouseIsInWidget because it is a child of a window
	ImVec2 columnStartPos = ImGui::GetCursorScreenPos();
	ImVec2 columnSize = ImVec2(ImGui::GetColumnWidth(), ImGui::GetContentRegionAvail().y);

	ImVec2 mousePos = ImGui::GetMousePos();
	isMouseInSecondColumn = (mousePos.x >= columnStartPos.x && mousePos.x <= (columnStartPos.x + columnSize.x) &&
		mousePos.y >= columnStartPos.y && mousePos.y <= (columnStartPos.y + columnSize.y));
}

void EditorIMGUI::ShowEntityComponents(glm::vec2 mousePos, Entity* displayedEntity, Manager& manager)
{

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	if (!displayedEntity) return;


	std::string windowTitle = "Entity";
	Node* node = dynamic_cast<Node*>(displayedEntity);
	Link* link = dynamic_cast<Link*>(displayedEntity);
	Empty* empty = dynamic_cast<Empty*>(displayedEntity);


	if (node) {
		windowTitle = "Node Display";
	}
	else if (link) {
		windowTitle = "Link Display";
	}
	else if (empty) {
		windowTitle = "Empty Display";
	}

	int currentId = displayedEntity->getId();

	if (currentId != _lastEntityDisplayed) {
		ImGui::SetNextWindowPos(ImVec2(mousePos.x, mousePos.y), ImGuiCond_Always);
		_lastEntityDisplayed = currentId;
	}

	if (ImGui::Begin(windowTitle.c_str())) {
		displayedEntity->imgui_print();

		if (node) {
			_data.SetSelectData(std::move(_pollingFileNames));

			if (ImGui::ComboAutoSelect("Select File For Polling", _data)) {
			}
			float buttonWidth = 100;
			if (ImGui::Button("Start Polling Sending Messages", ImVec2(buttonWidth, 0))) {
				std::string selectedFile = _data.input;
				if (!selectedFile.empty()) {
					StartPollingComponent(displayedEntity, selectedFile);
				}
			}
		}
		ImGui::Separator();

		std::unordered_map<std::string, size_t> componentOrder;
		size_t index = 0;
		for (const auto& pair : addComponentMap) {
			componentOrder[pair.first] = index++;
		}

		for (auto& [key, nameVec] : manager.componentNames) {
			std::sort(nameVec.begin(), nameVec.end(), [&](const std::string& a, const std::string& b) {
				return componentOrder[a] < componentOrder[b];
				});
		}

		if (node)
		{
			for (auto& c : manager.componentNames["Component"]) {
				// Checkbox to add/remove component
				bool hasComponent = displayedEntity->hasComponentByName(c);

				if (ImGui::Checkbox(c.c_str(), &hasComponent)) {
					if (hasComponent) {
						AddComponentByName(c, displayedEntity);
					}
					else {
						RemoveComponentByName(c, displayedEntity);
					}
				}

				if (hasComponent) {
					if (ImGui::TreeNode((c + " Properties").c_str())) {
						
						if (c == "SpriteComponent") {
							std::string tempStr = displayedEntity->GetComponent<SpriteComponent>().texture_name;
							getComponentByName(c, displayedEntity)->showGUI();
							ImGui::TreePop();

							if (tempStr != displayedEntity->GetComponent<SpriteComponent>().texture_name) {
								displayedEntity->getManager()->refresh(main_camera2D.get());
							}

						}
						else {
							getComponentByName(c, displayedEntity)->showGUI();
							ImGui::TreePop();
						}
					}
				}
			}
			for (auto& c : manager.componentNames["NodeComponent"]) {
				// Checkbox to add/remove component
				bool hasComponent = displayedEntity->hasComponentByName(c);

				if (ImGui::Checkbox(c.c_str(), &hasComponent)) {
					if (hasComponent) {
						AddComponentByName(c, displayedEntity);
					}
					else {
						RemoveComponentByName(c, displayedEntity);
					}
				}

				if (hasComponent) {
					if (ImGui::TreeNode((c + " Properties").c_str())) {
						getComponentByName(c, displayedEntity)->showGUI();
						ImGui::TreePop();
					}
				}
			}
		}


		else if (link) {
			for (auto& c : manager.componentNames["LinkComponent"]) {
				// Checkbox to add/remove component
				bool hasComponent = displayedEntity->hasComponentByName(c);

				if (ImGui::Checkbox(c.c_str(), &hasComponent)) {
					if (hasComponent) {
						AddComponentByName(c, displayedEntity);
					}
					else {
						RemoveComponentByName(c, displayedEntity);
					}
				}

				if (hasComponent) {
					if (ImGui::TreeNode((c + " Properties").c_str())) {
						getComponentByName(c, displayedEntity)->showGUI();
						ImGui::TreePop();
					}
				}
			}
		}


		else if (empty) {
			for (auto& c : manager.componentNames["Component"]) {
				// Checkbox to add/remove component
				bool hasComponent = displayedEntity->hasComponentByName(c);

				if (ImGui::Checkbox(c.c_str(), &hasComponent)) {
					if (hasComponent) {
						AddComponentByName(c, displayedEntity);
					}
					else {
						RemoveComponentByName(c, displayedEntity);
					}
				}

				if (hasComponent) {
					if (ImGui::TreeNode((c + " Properties").c_str())) {

						if (c == "SpriteComponent") {
							std::string tempStr = displayedEntity->GetComponent<SpriteComponent>().texture_name;
							getComponentByName(c, displayedEntity)->showGUI();
							ImGui::TreePop();

							if (tempStr != displayedEntity->GetComponent<SpriteComponent>().texture_name) {
								displayedEntity->getManager()->refresh(main_camera2D.get());
							}

						}
						else {
							getComponentByName(c, displayedEntity)->showGUI();
							ImGui::TreePop();
						}
					}
				}
			}
		}
	}

	ImGui::End();

}

void EditorIMGUI::ShowSceneControl(glm::vec2 mousePos, Manager& manager)
{
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	std::string windowTitle = "Scene Manager";

	static int sceneMan_nodeID1 = -1, sceneMan_nodeID2 = -1;
	static std::string errorMessage = "";

	ImGui::SetNextWindowPos(ImVec2(mousePos.x, mousePos.y), ImGuiCond_Always, ImVec2(0, 0));

	if (ImGui::Begin(windowTitle.c_str())) {
		if (ImGui::Button("Create Empty - Box")) {
			auto& empty(manager.addEntityNoId<Empty>());

			glm::vec2 position(0, 0);

			empty.addComponent<TransformComponent>(position, Layer::action, glm::vec3(10.0f), 1);

			empty.addComponent<BoxComponent>();

			manager.grid->addEmpty(&empty, manager.grid->getGridLevel());
			empty.addGroup(Manager::groupEmpties);
			manager.aboutTo_updateActiveEntities();
			// todo here we also have choose shape option
		}

		if (ImGui::Button("Create Empty - Plane")) {
			auto& empty(manager.addEntityNoId<Empty>());

			//empty.addGroup(Manager::groupNodes_0);
			// todo here we also have choose shape option
		}

		if (ImGui::Button("Create Empty - Triangle")) {
			auto& empty(manager.addEntityNoId<Empty>());

			//empty.addGroup(Manager::groupNodes_0);
			// todo here we also have choose shape option
		}

		if (ImGui::Button("Create Empty - Sphere")) {
			auto& empty(manager.addEntityNoId<Empty>());

			//empty.addGroup(Manager::groupNodes_0);
			// todo here we also have choose shape option
		}

		ImGui::Separator();

		if (ImGui::Button("Create Node Entity")) {
			auto& node(manager.addEntityNoId<Node>());

			glm::vec2 position(0, 0);

			node.addComponent<TransformComponent>(position, Layer::action, glm::vec3(10.0f), 1);
			node.addComponent<Rectangle_w_Color>();
			node.GetComponent<Rectangle_w_Color>().color = Color(150, 150, 150, 255);

			node.GetComponent<TransformComponent>().update(0.0f); // update children positions


			manager.grid->addNode(&node, manager.grid->getGridLevel());
			node.addGroup(Manager::groupNodes_0);
			manager.aboutTo_updateActiveEntities();
		}

		ImGui::Separator();
		ImGui::Text("Create Link Entity");
		ImGui::InputInt("Node ID 1", &sceneMan_nodeID1);
		ImGui::InputInt("Node ID 2", &sceneMan_nodeID2);

		if (ImGui::Button("Create Link Entity")) {
			// Check if both nodes exist before creating the link
			if (manager.getEntityFromId(sceneMan_nodeID1) && manager.getEntityFromId(sceneMan_nodeID2)) {
				auto& link(manager.addEntityNoId<Link>(sceneMan_nodeID1, sceneMan_nodeID2));

				
				link.addComponent<Line_w_Color>();

				link.GetComponent<Line_w_Color>().setSrcColor(Color(255, 40, 0, 255));
				link.GetComponent<Line_w_Color>().setDestColor(Color(40, 255, 0, 255));

				link.addComponent<LineFlashAnimatorComponent>();


				link.addGroup(Manager::groupLinks_0);
				manager.grid->addLink(&link, manager.grid->getGridLevel());
				manager.aboutTo_updateActiveEntities();
				errorMessage = ""; // Clear error if successful
			}
			else {
				errorMessage = "Error: One or both node IDs do not exist!";
			}
		}
	}

	ImGui::End();

}

void EditorIMGUI::StartPollingComponent(Entity* entity, const std::string& fileName) {
	if (!entity) return;

	// Attach a polling component to the entity
	if (!entity->hasComponent<PollingComponent>()) {
		entity->addComponent<PollingComponent>();
	}

	entity->GetComponent<PollingComponent>().StartPolling(fileName, 10.0f);

}
