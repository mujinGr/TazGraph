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

void EditorIMGUI::BackGroundUIElement(bool &renderDebug, glm::vec2 mouseCoords, const Manager& manager, Entity* selectedEntity, float(& backgroundColor)[4], int cell_size) {
	ImGui::Begin("Background UI");
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

	ImGui::Text("Mouse Coords: {x: %f, y: %f}", mouseCoords.x, mouseCoords.y);

	if (selectedEntity) {
		ImGui::Text("Selected Entity Details");

		// Example: Display components of the selected entity
		if (selectedEntity->hasComponent<TransformComponent>()) {
			TransformComponent* tr = &selectedEntity->GetComponent<TransformComponent>();
			ImGui::Text("Position: (%f, %f)", tr->getPosition().x, tr->getPosition().y);
			ImGui::Text("Size: (%d, %d)", tr->width, tr->height);
			int cellX = (int)(std::floor(tr->getPosition().x / cell_size));
			int cellY = (int)(std::floor(tr->getPosition().y / cell_size));
			ImGui::Text("Grid index: %d with x: %d and y: %d", manager.grid->getCell(*selectedEntity), cellX, cellY);
		}
	}
	ImGui::End();
}

void EditorIMGUI::FileActions() {

	ImVec2 displaySize = ImGui::GetIO().DisplaySize;

	// Calculate the window size as a fraction of display size
	float windowWidth = displaySize.x * 0.15f;  // 40% of the application window's width
	float windowHeight = displaySize.y * 0.08f; // 30% of the application window's height

	// Set the next window size
	ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(0.0f,0.0f), ImGuiCond_Once);

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoTitleBar;

	ImGui::Begin("File Actions", NULL, windowFlags);
	if (ImGui::Button("Save", ImVec2(-1.0f, 0.0f))) {
		_isSaving = true;
	}
	if (ImGui::Button("Load", ImVec2(-1.0f, 0.0f))) {
		_isLoading = true;
	}
	if (ImGui::Button("Back", ImVec2(-1.0f, 0.0f))) {
		_goingBack = true;
	}
	ImGui::End();
}

void EditorIMGUI::FPSCounter(const BaseFPSLimiter& baseFPSLimiter) {

	ImGui::Begin("Performance");
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

		ImPlot::SetupAxesLimits(0, 100, 0, 70);

		ImPlot::PlotLine("FPS", &baseFPSLimiter.fpsHistory[0], plot_count);

		ImPlot::EndPlot();
	}
	ImGui::End();

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

void EditorIMGUI::ShowAllEntities(Manager& manager) {
	ImGui::Begin("Entity List");

	for (std::size_t group = Manager::groupBackgroundLayer; group != Manager::buttonLabels; group++) {
		std::string s = manager.getGroupName(group);
		ImGui::Text("Group: %s", s.c_str());

		std::vector<Entity*>& groupVec = manager.getGroup(group);
		for (auto& entity : groupVec) {

			ImGui::Text("Entity ID: %d", entity->getId());

		}
	}

	ImGui::End();
}
