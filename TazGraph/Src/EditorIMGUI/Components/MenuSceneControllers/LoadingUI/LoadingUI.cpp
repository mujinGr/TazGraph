#include "LoadingUI.h"

void LoadingUI::OnImGuiRender()
{
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 windowSize(400, 100); // Desired window size
	ImVec2 windowPos((io.DisplaySize.x - windowSize.x) * 0.5f,
		(io.DisplaySize.y - windowSize.y) * 0.5f);

	ImGui::SetNextWindowPos(windowPos);
	ImGui::SetNextWindowSize(windowSize);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;
	bool open = true;
	ImGui::Begin("Loading...", &open, window_flags);

	DataManager::getInstance().ReloadAccessibleFiles();

	DataManager::getInstance().data.SetSelectData(std::move(DataManager::getInstance().fileNames));

	if (ImGui::ComboAutoSelect("Select File", DataManager::getInstance().data)) {
	}
	ImGui::NewLine();
	float windowWidth = ImGui::GetContentRegionAvail().x;
	float buttonWidth = 100; // Define the button width you want
	ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f); // Center the button

	if (ImGui::Button("Load", ImVec2(buttonWidth, 0))) {
		DataManager::getInstance().loading = false;
	}

	if (!open) {
		std::memset(DataManager::getInstance().data.input, 0, sizeof(DataManager::getInstance().data.input));
		DataManager::getInstance().loading = false;
	}

	ImGui::End();
}
