#include "EditorIMGUI.h"

EditorIMGUI::EditorIMGUI() {

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

void EditorIMGUI::BackGroundUIElement(bool &renderDebug, glm::vec2 mouseCoords, glm::vec2 mouseCoords2, Manager& manager, Entity* selectedEntity, float(& backgroundColor)[4], int cell_size) {
	ImGui::BeginChild("Background UI");
	ImGui::Text("This is a Background UI element.");
	ImGui::ColorEdit4("Background Color", backgroundColor);
	// Change color based on the debug mode state
	if (renderDebug) {
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));  // Green for ON
	}
	else {
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.2f, 0.2f, 1.0f));  // Red for OFF
	}

	// Button toggles the debug mode
	if (ImGui::Button("Enable Debug Mode")) {
		renderDebug = !renderDebug;  // Toggle the state
	}

	ImGui::PopStyleColor(1);
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
		for (std::size_t managerGroup = Manager::groupBackgroundLayer; managerGroup != Manager::cursorGroup + 1; ++managerGroup) {
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("%s", manager.getGroupName(managerGroup).c_str());
			ImGui::TableSetColumnIndex(1);
			int groupSize = manager.getVisibleGroup(managerGroup).size();
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
		ImGui::Text("visible nodes");
		ImGui::TableSetColumnIndex(1);
		ImGui::Text("%zu", manager.getVisibleNodes().size());

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("visible links");
		ImGui::TableSetColumnIndex(1);
		ImGui::Text("%zu", manager.getVisibleLinks().size());
		
		ImGui::EndTable();
	}


	ImGui::Text("Scene/Screen Coords: {x: %f, y: %f}", mouseCoords.x, mouseCoords.y);
	ImGui::Text("MainViewport Coords: {x: %f, y: %f}", mouseCoords2.x, mouseCoords2.y);


	if (selectedEntity) {
		ImGui::Text("Selected Entity Details");

		if (selectedEntity->hasComponent<TransformComponent>()) {

			ImGui::Text("Id: %d", selectedEntity->getId());

			TransformComponent* tr = &selectedEntity->GetComponent<TransformComponent>();
			ImGui::Text("Position: (%f, %f)", tr->getPosition().x, tr->getPosition().y);
			ImGui::Text("Size: (%f, %f)", tr->bodyDims.w, tr->bodyDims.h);
			SDL_FRect cellBox = manager.grid->getCell(*selectedEntity, manager.grid->getGridLevel())->boundingBox;
			ImGui::Text("Grid x: %.2f and y: %.2f", cellBox.x, cellBox.y);
		}
	}
	ImGui::EndChild();
}

void EditorIMGUI::FPSCounter(const BaseFPSLimiter& baseFPSLimiter) {

	ImGui::BeginChild("Performance");
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
	ImGui::EndChild();

}

void EditorIMGUI::ReloadAccessibleFiles() {
	if (!_filesLoaded) {
		_fileNames.clear();
		const std::string path = "assets/Maps"; // Directory path
		for (const auto& entry : fs::directory_iterator(path)) {
			if (entry.is_regular_file()) {
				_fileNames.push_back(entry.path().filename().string()); // Add file name to vector
			}
		}
		_filesLoaded = true; // Set to true so we don't reload unnecessarily
	}
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
	ImGui::BeginChild("Tab 2");

	ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	float size = 10;

	for (std::size_t group = Manager::groupBackgroundLayer; group != Manager::cursorGroup + 1; group++) {
		std::string s = manager.getGroupName(group);

		if (ImGui::CollapsingHeader(s.c_str())) {
			std::vector<Entity*>& groupVec = manager.getGroup(group);

			if ( group == Manager::groupNodes_0 || group == Manager::groupGroupNodes_0 || group == Manager::groupGroupNodes_1 || group == Manager::cursorGroup) {

				for (auto& entity : groupVec) { // loops 1 time
					Color initialColor = entity->GetComponent<Rectangle_w_Color>().color;
					color = ImVec4(initialColor.r / 255.0f, initialColor.g / 255.0f, initialColor.b / 255.0f, initialColor.a / 255.0f);

					TransformComponent* tr = &entity->GetComponent<TransformComponent>();
					float initialSize[2] = { tr->bodyDims.w, tr->bodyDims.h };
					size = initialSize[0];
					break;
				}

				if (ImGui::ColorEdit4(("Color##" + s).c_str(), (float*)&color)) {
					std::vector<Entity*>& groupVec = manager.getGroup(group);
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
							entity->GetComponent<TransformComponent>().bodyDims.w = size;
							entity->GetComponent<TransformComponent>().bodyDims.h = size;
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
					std::vector<Entity*>& groupVec = manager.getGroup(group);
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

						entity->imgui_print();

						ImGui::TreePop();
					}

				}
				ImGui::TreePop();
			}
			
		}
	}

	ImGui::EndChild();
}


void EditorIMGUI::SceneViewport(uint32_t textureId, ImVec2& storedWindowPos, ImVec2& storedWindowSize) {
	ImGui::BeginChild("Viewport");
	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	//ImGui::Image();
	ImGui::Image(
		(void*)textureId,
		viewportPanelSize,
		ImVec2(0, 1),
		ImVec2(1, 0)
	);

	storedWindowPos = ImGui::GetWindowPos(); // You need to call this when the window is in context (i.e., between ImGui::Begin and ImGui::End)
	storedWindowSize = viewportPanelSize;

	ImGui::EndChild();
}

std::string EditorIMGUI::SceneTabs() {
	ImGui::Begin("Scene Tabs");
	
	std::vector<std::string> scenes = { "test_links.txt", "test_medium" };

	if (ImGui::BeginTabBar("SceneTabs")) {
		for (int i = 0; i < scenes.size(); i++) {
			// Check if the tab is selected
			bool isSelected = ( i == 1);
			if (ImGui::BeginTabItem(scenes[i].c_str(), nullptr, isSelected ? ImGuiTabItemFlags_SetSelected : 0)) {
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
	return scenes[0];
}

void EditorIMGUI::ShowStatisticsAbout(glm::vec2 mousePos, Entity* displayedEntity)
{
	if (!displayedEntity) return;

	ImGui::SetNextWindowPos(ImVec2(mousePos.x, mousePos.y), ImGuiCond_Always, ImVec2(0, 0));

	if (ImGui::Begin("Display Entity Statistics")) {
		displayedEntity->imgui_print();
	}

	ImGui::End();

}
