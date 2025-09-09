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
			getSubcomponent<PythonInterpreterPanel>()->showPythonInterpreter = !getSubcomponent<PythonInterpreterPanel>()->showPythonInterpreter;
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Camera")) {
				getSubcomponent<CameraPanel>()->showCameraPanel=
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


		ImGui::EndMenuBar();
	}


	if (getSubcomponent<PythonInterpreterPanel>()->showPythonInterpreter)
	{
		getSubcomponent<PythonInterpreterPanel>()->setConfig({});
		getSubcomponent<PythonInterpreterPanel>()->OnImGuiRender();
	}

	if (getSubcomponent<CameraPanel>()->showCameraPanel) {
		getSubcomponent<CameraPanel>()->setConfig({});
		getSubcomponent<CameraPanel>()->OnImGuiRender();
	}
	if (getSubcomponent<FPSCounter>()->showFPS) {
		getSubcomponent<FPSCounter>()->setLimiter(config.scene->getApp()->getFPSLimiter());
		getSubcomponent<FPSCounter>()->OnImGuiRender();
	}
	if (getSubcomponent<VisibleEntitiesPanel>()->showVisibleEntities) {
		getSubcomponent<VisibleEntitiesPanel>()->setConfig({config.scene->manager});
		getSubcomponent<VisibleEntitiesPanel>()->OnImGuiRender();
	}

}
