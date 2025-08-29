#include "MenuDropdown.h"

void MenuDropdownPanel::update(float deltaTime)
{
	loadingUI.update();
	savingUI.update();
	newMapUI.update();
}

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

		ImGui::EndMenuBar();
	}

}
