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

void EditorIMGUI::setLoading(bool loading)
{
	_isLoading = loading;
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

void EditorIMGUI::BackGroundUIElement(bool &renderDebug, glm::vec2 mouseCoords, glm::vec2 mouseCoords2, Manager& manager, Entity* onHoverEntity, float(& backgroundColor)[4], int cell_size) {
	ImGui::BeginChild("Background UI");
	ImGui::Text("This is a Background UI element.");
	ImGui::ColorEdit4("Background Color", backgroundColor);
	
	ImVec4 activeColor = ImVec4(0.2f, 0.7f, 0.2f, 1.0f);
	ImVec4 defaultColor = ImVec4(0.0f, 0.5f, 1.0f, 1.0f);
	
	// Change color based on the debug mode state
	if (renderDebug) {
		ImGui::PushStyleColor(ImGuiCol_Button, activeColor);  // Green for ON
	}
	else {
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.2f, 0.2f, 1.0f));  // Red for OFF
	}

	// Button toggles the debug mode
	if (ImGui::Button("Enable Debug Mode")) {
		renderDebug = !renderDebug;  // Toggle the state
	}

	ImGui::PopStyleColor(1);
	
	ImGui::Separator();

	if (ImGui::Button(arrowheadsEnabled ? "Disable Arrowheads" : "Enable Arrowheads")) {
		arrowheadsEnabled = !arrowheadsEnabled;
		//manager.setArrowheadsEnabled(arrowheadsEnabled); // Call function to apply change
	}
	
	ImGui::Text("Select Grouping Layout:");
	ImGui::PushStyleColor(ImGuiCol_Button, activeLayout == 0 ? activeColor : defaultColor);
	if (ImGui::Button("Default Layout", ImVec2(120, 30))) {
		activeLayout = 0;
		//manager.setGroupingLayout(GroupingLayout::Grid);
	}
	ImGui::PopStyleColor();

	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Button, activeLayout == 1 ? activeColor : defaultColor);
	if (ImGui::Button("Group Layout 1", ImVec2(120, 30))) {
		activeLayout = 1;
		//manager.setGroupingLayout(GroupingLayout::Circular);
	}
	ImGui::PopStyleColor();

	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Button, activeLayout == 2 ? activeColor : defaultColor);
	if (ImGui::Button("Group Layout 2", ImVec2(120, 30))) {
		activeLayout = 2;
		//manager.setGroupingLayout(GroupingLayout::ForceDirected);
	}
	ImGui::PopStyleColor();

	ImGui::Text("Camera Position");

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

	if (ImGui::Button("Reset")) {
		main_camera2D->resetCameraPosition();  // Toggle the state
	}

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

std::string EditorIMGUI::SceneTabs() {
	float childHeight = 30.0f;
	
	ImGui::BeginChild("Scene Tabs", ImVec2(0, childHeight), true, ImGuiWindowFlags_NoScrollbar);
	
	static std::vector<bool> closeTab(_fileNames.size(), false);

	if (ImGui::BeginTabBar("SceneTabs")) {
		for (int i = 0; i < _fileNames.size(); i++) {
			if (closeTab[i]) continue;

			if (ImGui::BeginTabItem(_fileNames[i].c_str(), nullptr, ImGuiTabItemFlags_None)) {
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}

	for (int i = _fileNames.size() - 1; i >= 0; i--) {
		if (closeTab[i]) {
			closeTab.erase(closeTab.begin() + i);
		}
	}

	ImGui::EndChild();
	return !_fileNames.empty() ? _fileNames.front() : "nullptr";
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

void EditorIMGUI::ShowStatisticsAbout(glm::vec2 mousePos, Entity* displayedEntity, Manager& manager)
{

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	if (!displayedEntity) return;


	std::string windowTitle = "Entity";
	Node* node = dynamic_cast<Node*>(displayedEntity);
	Link* link = dynamic_cast<Link*>(displayedEntity);

	if (node) {
		windowTitle = "Node Display";
	}
	else if (link) {
		windowTitle = "Link Display";
	}


	ImGui::SetNextWindowPos(ImVec2(mousePos.x, mousePos.y), ImGuiCond_Always, ImVec2(0, 0));

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
		if (link) {
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
		if (ImGui::Button("Create Empty Entity")) {
			auto& empty(manager.addEntityNoId<Empty>());

			//empty.addGroup(Manager::groupNodes_0);
			// todo here we also have choose shape option
		}

		if (ImGui::Button("Create Node Entity")) {
			auto& node(manager.addEntityNoId<Node>());

			glm::vec2 position(0, 0);

			node.addComponent<TransformComponent>(position, Layer::action, glm::vec3(10.0f), 1);
			node.addComponent<Rectangle_w_Color>();
			node.GetComponent<Rectangle_w_Color>().color = Color(150, 150, 150, 255);

			node.GetComponent<TransformComponent>().update(0.0f); // update children positions


			manager.grid->addNode(&node, manager.grid->getGridLevel());
			node.addGroup(Manager::groupNodes_0);
			manager.refresh(main_camera2D.get());
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
				manager.refresh(main_camera2D.get());
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
