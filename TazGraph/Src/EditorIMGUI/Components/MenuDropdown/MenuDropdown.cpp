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

		if (ImGui::BeginMenu("Python Interpreter"))
		{
			pythonInterpreter.showPythonInterpreter = !pythonInterpreter.showPythonInterpreter;
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Camera")) {

			}
			if (ImGui::MenuItem("FPS Counter")) {

			}
			if (ImGui::MenuItem("Visible Entities Table")) {

			}
			ImGui::EndMenu();
		}


		ImGui::EndMenuBar();
	}


	if (pythonInterpreter.showPythonInterpreter)
	{
		pythonInterpreter.setConfig({});
		pythonInterpreter.update();
		pythonInterpreter.OnImGuiRender();
	}
}
