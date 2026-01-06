#include "MainMenuEditorLayer.h"

void MainMenuEditorLayer::OnImGuiRender()
{
	// Constants
	const float BUTTON_WIDTH = 240;
	const float BUTTON_HEIGHT = 45;
	const float BUTTON_SPACING = 10;
	const float TOP_PADDING = 20;
	const float BOTTOM_PADDING = 20;
	const float SIDE_PADDING = 20;
	const int BUTTON_COUNT = 3; // Start New, Load, Exit

	// Calculate exact window size
	const float WINDOW_WIDTH = BUTTON_WIDTH + (SIDE_PADDING * 2);
	const float WINDOW_HEIGHT = (BUTTON_HEIGHT * BUTTON_COUNT) +
		(BUTTON_SPACING * (BUTTON_COUNT - 1)) +
		TOP_PADDING + BOTTOM_PADDING;

	ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f),
		ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT));

	ImGuiInterface::StyleColorsCustom(&ImGui::GetStyle());

	ImGui::Begin("Control Window", NULL,
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse);

	ImGui::SetWindowFontScale(1.3f);

	float buttonPosX = SIDE_PADDING;
	TextureManager& tm = TextureManager::getInstance();

	ImFont* boldFont = tm.Get_Font("bold");
	if (boldFont) ImGui::PushFont(boldFont);
	// Start New Button
	ImGui::SetCursorPos(ImVec2(buttonPosX, TOP_PADDING));
	if (ImGui::Button("Start New", ImVec2(BUTTON_WIDTH, BUTTON_HEIGHT))) {
		config.onStartClicked();
	}

	// Load Button
	ImGui::SetCursorPos(ImVec2(buttonPosX, TOP_PADDING + BUTTON_HEIGHT + BUTTON_SPACING));
	if (ImGui::Button("Load", ImVec2(BUTTON_WIDTH, BUTTON_HEIGHT))) {
		DataManager::getInstance().setLoading(true);
	}

	// Exit Button
	ImGui::SetCursorPos(ImVec2(buttonPosX, TOP_PADDING + (BUTTON_HEIGHT + BUTTON_SPACING) * 2));
	if (ImGui::Button("Exit", ImVec2(BUTTON_WIDTH, BUTTON_HEIGHT))) {
		config.onExitClicked();
	}
	if (boldFont) ImGui::PopFont();
	ImFont* arialFont = tm.Get_Font("arial");
	if (arialFont) ImGui::PushFont(arialFont);
	if (boldFont) ImGui::PopFont();

	ImGui::SetWindowFontScale(1.0f);
	ImGui::End();
}
