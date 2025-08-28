#include "SavingUI.h"

void SavingUI::OnImGuiRender()
{
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 windowSize(400, 100); // Desired window size
	ImVec2 windowPos((io.DisplaySize.x - windowSize.x) * 0.5f,
		(io.DisplaySize.y - windowSize.y) * 0.5f);

	ImGui::SetNextWindowPos(windowPos);
	ImGui::SetNextWindowSize(windowSize);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;
	bool open = true;
	ImGui::Begin("Saving...", &open, window_flags);

	DataManager::getInstance().ReloadAccessibleFiles();

	DataManager::getInstance().data.SetSelectData(std::move(DataManager::getInstance().fileNames));

	if (ImGui::ComboAutoSelect("Select File", DataManager::getInstance().data)) {
	}
	ImGui::NewLine();
	float windowWidth = ImGui::GetContentRegionAvail().x;
	float buttonWidth = 100; // Define the button width you want
	ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f); // Center the button

	if (ImGui::Button("Save", ImVec2(buttonWidth, 0))) {
		config.c_map->saveMapAsText(DataManager::getInstance().data.input); // save Map that is selected
		DataManager::getInstance().saving = false;
		DataManager::getInstance().filesLoaded = false;
	}

	if (!open) {
		DataManager::getInstance().saving = false;
	}

	ImGui::End();
}
