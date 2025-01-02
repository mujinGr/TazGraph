#include "EditorIMGUI.h"

EditorIMGUI::EditorIMGUI() {

}

EditorIMGUI::~EditorIMGUI() {

}

void EditorIMGUI::BackGroundUIElement() {
	//ImGui::Begin("Background UI");
	//ImGui::Text("This is a Background UI element.");
	//ImGui::ColorEdit4("Background Color", backgroundColor);
	//// Change color based on the debug mode state
	//if (renderDebug) {
	//	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));  // Green for ON
	//}
	//else {
	//	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.2f, 0.2f, 1.0f));  // Red for OFF
	//}

	//// Button toggles the debug mode
	//if (ImGui::Button("Enable Debug Mode")) {
	//	renderDebug = !renderDebug;  // Toggle the state
	//}

	//ImGui::PopStyleColor(1);
	//ImGui::Text("Camera Position");

	//std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	//ImGui::Text("Rect: {x: %f, y: %f, w: %f, h: %f}", main_camera2D->getCameraRect().x, main_camera2D->getCameraRect().y, main_camera2D->getCameraRect().w, main_camera2D->getCameraRect().h);

	//if (ImGui::SliderFloat3("Eye Position", &main_camera2D->eyePos[0], -1000.0f, 1000.0f)) {
	//	main_camera2D->setCameraMatrix(glm::lookAt(main_camera2D->eyePos, main_camera2D->aimPos, main_camera2D->upDir));
	//}
	//if (ImGui::SliderFloat3("Aim Position", &main_camera2D->aimPos[0], -1000.0f, 1000.0f)) {
	//	main_camera2D->setCameraMatrix(glm::lookAt(main_camera2D->eyePos, main_camera2D->aimPos, main_camera2D->upDir));
	//}
	//if (ImGui::SliderFloat3("Up Direction", &main_camera2D->upDir[0], -1000.0f, 1000.0f)) {
	//	main_camera2D->setCameraMatrix(glm::lookAt(main_camera2D->eyePos, main_camera2D->aimPos, main_camera2D->upDir));
	//}

	//ImGui::Text("Mouse Coords: {x: %f, y: %f}", _graph->_inputManager.getMouseCoords().x, _graph->_inputManager.getMouseCoords().y);

	//if (_selectedEntity) {
	//	ImGui::Text("Selected Entity Details");

	//	// Example: Display components of the selected entity
	//	if (_selectedEntity->hasComponent<TransformComponent>()) {
	//		TransformComponent* tr = &_selectedEntity->GetComponent<TransformComponent>();
	//		ImGui::Text("Position: (%f, %f)", tr->getPosition().x, tr->getPosition().y);
	//		ImGui::Text("Size: (%d, %d)", tr->width, tr->height);
	//		int cellX = (int)(std::floor(tr->getPosition().x / CELL_SIZE));
	//		int cellY = (int)(std::floor(tr->getPosition().y / CELL_SIZE));
	//		ImGui::Text("Grid index: %d with x: %d and y: %d", manager.grid->getCell(*_selectedEntity), cellX, cellY);
	//	}
	//}
	//ImGui::End();
}

void EditorIMGUI::FileActions(Map* map) {
	ImGui::Begin("File Actions");
	if (ImGui::Button("Save", ImVec2(-1.0f, 0.0f))) {
		map->saveMapAsText("dummy_graph");
		//_showSaveWindow = true;
	}
	if (ImGui::Button("Load", ImVec2(-1.0f, 0.0f))) {
		// Code to load a state
	}
	if (ImGui::Button("Back", ImVec2(-1.0f, 0.0f))) {
		// Code to go back or close the window
	}
	ImGui::End();
}

void EditorIMGUI::FPSCounter(const BaseFPSLimiter& baseFPSLimiter) {

	ImGui::Begin("Performance");
	if (ImPlot::BeginPlot("FPS Plot")) {
#if defined(_WIN32) || defined(_WIN64)
		int plot_count = std::min(baseFPSLimiter.fps_history_count,
			baseFPSLimiter.fpsHistoryIndx); // Ensuring we do not read out of bounds
		int plot_offset = std::max(0,
			baseFPSLimiter.fpsHistoryIndx - baseFPSLimiter.fps_history_count); // Ensure a positive offset
#else
		int plot_count = min(baseFPSLimiter->fps_history_count,
			baseFPSLimiter->fpsHistoryIndx); // Ensuring we do not read out of bounds
		int plot_offset = max(0,
			baseFPSLimiter->fpsHistoryIndx - baseFPSLimiter->fps_history_count); // Ensure a positive offset

#endif

		ImPlot::SetupAxesLimits(0, 100, 0, 70);

		ImPlot::PlotLine("FPS", &baseFPSLimiter.fpsHistory[0], plot_count);

		ImPlot::EndPlot();
	}
	ImGui::End();

}