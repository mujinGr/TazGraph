#include "GraphMiddlePanel.h"

void GraphMiddlePanel::OnImGuiRender()
{
	std::string activeManagerKey = config.scene->managerName;
	getSubcomponent<GraphTopBar>()->setConfig(
		{
			.scene = config.scene
		});
	getSubcomponent<GraphTopBar>()->OnImGuiRender();

	std::string closedTab = getSubcomponent<GraphTopBar>()->
		getTabToClose();
	if (!closedTab.empty()) {
		auto managerIt = config.scene->managers.find(closedTab);
		if (managerIt != config.scene->managers.end()) {
			config.scene->managers.erase(managerIt);

			if (closedTab == activeManagerKey) {
				if (!config.scene->managerName.empty() &&
					config.scene->managers.find(config.scene->managerName) != config.scene->managers.end()) {
					config.setManager(config.scene->managerName);
				}
				else if (!config.scene->managers.empty()) {
					config.setManager(config.scene->managers.begin()->first);
				}
				else {
					activeManagerKey = "";
				}
			}
		}
	}
	else if (activeManagerKey != config.scene->managerName && !config.scene->managerName.empty()) {
		// Normal tab switching (no closure)
		auto managerIt = config.scene->managers.find(config.scene->managerName);
		if (managerIt != config.scene->managers.end()) {
			config.setManager(config.scene->managerName);
		}
	}
	float viewportHeight = ImGui::GetContentRegionAvail().y;
	if (!getSubcomponent<PythonInterpreterPanel>()->isCollapsed) {
		viewportHeight *= 0.55f;
	}
	static bool wasPythonPanelCollapsed = false;
	bool isPythonPanelCollapsed = getSubcomponent<PythonInterpreterPanel>()->isCollapsed;
	ImGuiChildFlags flags = ImGuiChildFlags_ResizeY;

	if (wasPythonPanelCollapsed != isPythonPanelCollapsed) {
		// Panel state changed, force the new size
		flags = 0; // Remove resize flag temporarily
		wasPythonPanelCollapsed = isPythonPanelCollapsed;
	}

	ImGui::BeginChild("Viewport", ImVec2(0.0f, viewportHeight - getSubcomponent<PythonInterpreterPanel>()->buttonSize.y), flags);

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	TazGraphEngine::drawTextAtWorldPositionPerspective(glm::vec3(0), "asdasd", glm::vec4(1.0f), main_camera2D.get());

	getSubcomponent<ViewportPanel>()->setConfig({
		.c_fb = config.c_framebuffer,
		.c_minimap_fb = config.c_minimapFramebuffer,
		.c_storedWindowPos = config.c_windowPos,
		.c_storedWindowSize = config.c_windowSize,
		.startPos = config.startPos,
		.currPos = config.currPos,
		});
	getSubcomponent<ViewportPanel>()->OnImGuiRender();
	ImGui::EndChild();


	getSubcomponent<PythonInterpreterPanel>()->setConfig({ .scene = config.scene });
	getSubcomponent<PythonInterpreterPanel>()->OnImGuiRender();

}
