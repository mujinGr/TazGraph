#include "NewMapUI.h"

void NewMapUI::OnImGuiRender()
{
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
		DataManager::getInstance().startingNew = false;
	}

	if (!open) {
		DataManager::getInstance().startingNew = false;
	}

	ImGui::End();
}
