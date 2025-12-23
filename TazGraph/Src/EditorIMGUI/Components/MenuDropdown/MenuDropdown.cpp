#include "MenuDropdown.h"

void MenuDropdownPanel::OnImGuiRender()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save")) {
				DataManager::getInstance().saving = true;
			}
			if (ImGui::MenuItem("Load")) {
				DataManager::getInstance().loading = true;
			}
			if (ImGui::MenuItem("Back")) {
				DataManager::getInstance().goingBack = true;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Scene")) {
				getSubcomponent<ScenePanel>()->showScenePanel =
					!getSubcomponent<ScenePanel>()->showScenePanel;
			}
			if (ImGui::MenuItem("Camera")) {
				getSubcomponent<CameraPanel>()->showCameraPanel =
					!getSubcomponent<CameraPanel>()->showCameraPanel;
			}
			if (ImGui::MenuItem("FPS Counter")) {
				getSubcomponent<FPSCounter>()->showFPS =
					!getSubcomponent<FPSCounter>()->showFPS;
			}
			if (ImGui::MenuItem("Visible Entities Table")) {
				getSubcomponent<VisibleEntitiesPanel>()->showVisibleEntities =
					!getSubcomponent<VisibleEntitiesPanel>()->showVisibleEntities;
			}
			ImGui::EndMenu();
		}
		ImTextureID icon = (ImTextureID)(intptr_t)TextureManager::getInstance().Get_GLTexture("infoIcon")->id;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));

		// Get the position before the menu
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		float menuHeight = ImGui::GetFrameHeight();

		if (ImGui::BeginMenu("Help    "))
		{
			getSubcomponent<HelpPanel>()->showHelpPanel =
				!getSubcomponent<HelpPanel>()->showHelpPanel;

			ImGui::EndMenu();
		}

		// Draw the icon over the reserved space
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec2 iconPos = ImVec2(cursorPos.x + 32, cursorPos.y + (menuHeight - 20) * 0.5f);
		drawList->AddImage(icon, iconPos, ImVec2(iconPos.x + 16, iconPos.y + 16));

		ImGui::PopStyleVar();

		ImGui::EndMenuBar();
	}

	if (getSubcomponent<ScenePanel>()->showScenePanel) {
		getSubcomponent<ScenePanel>()->setConfig({ .scene = config.scene });
		getSubcomponent<ScenePanel>()->OnImGuiRender();
	}
	if (getSubcomponent<CameraPanel>()->showCameraPanel) {
		getSubcomponent<CameraPanel>()->setConfig({});
		getSubcomponent<CameraPanel>()->OnImGuiRender();
	}
	if (getSubcomponent<FPSCounter>()->showFPS) {
		getSubcomponent<FPSCounter>()->setLimiter(config.scene->getApp()->getFPSLimiter());
		getSubcomponent<FPSCounter>()->OnImGuiRender();
	}
	if (getSubcomponent<HelpPanel>()->showHelpPanel) {
		getSubcomponent<HelpPanel>()->setConfig({});
		getSubcomponent<HelpPanel>()->OnImGuiRender();
	}
	if (getSubcomponent<VisibleEntitiesPanel>()->showVisibleEntities) {
		getSubcomponent<VisibleEntitiesPanel>()->setConfig({ config.scene->manager });
		getSubcomponent<VisibleEntitiesPanel>()->OnImGuiRender();
	}

}
