#include "GraphMiddlePanel.h"

void GraphMiddlePanel::OnImGuiRender()
{
	std::string activeManagerKey = config.scene->managerName;
	//	
	//	
	//	
	//	
	//	}
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

	getSubcomponent<ViewportPanel>()->setConfig({
		.c_fb = config.c_framebuffer,
		.c_minimap_fb = config.c_minimapFramebuffer,
		.c_storedWindowPos = config.c_windowPos,
		.c_storedWindowSize = config.c_windowSize
		});
	getSubcomponent<ViewportPanel>()->OnImGuiRender();

	ImGui::BeginChild("TEstTest");

	ImGui::Text("LMAO");

	ImGui::EndChild();
}
