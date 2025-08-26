#include "MainMenuPanel.h"

void MainMenuPanel::OnImGuiRender()
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
		config.onStartClicked();
	}

	ImGui::Dummy(ImVec2(0.0f, buttonSpacing));

	ImGui::SetCursorPosX(buttonPosX);
	if (ImGui::Button("Load", ImVec2(buttonWidth, buttonHeight))) {
		config.onLoadClicked();
	}

	ImGui::Dummy(ImVec2(0.0f, buttonSpacing));

	ImGui::SetCursorPosX(buttonPosX);
	if (ImGui::Button("Exit", ImVec2(buttonWidth, buttonHeight))) {
		config.onExitClicked();
	}

	ImGui::End();
}
