#include "GraphTopBar.h"

void GraphTopBar::update(float deltaTime) {
	UIElement::update(deltaTime);

	if (!config.scene) return; // not yet initialized by scene
	Manager* manager = config.scene->manager;

	int& current_simulation_step = manager->currentStep;


	if (interpolation_running && !manager->steps.empty()) {
		interpolation += interpolation_speed * deltaTime / config.scene->getApp()->getFPSLimiter().fps;

		// here, get interpolation and manager steps intervals
		if (
			current_simulation_step < manager->steps.size() &&
			interpolation >= manager->steps[current_simulation_step].timestamp) {
			// if step done, then apply step (function at GECS)
			// where it will change the color/size/pos of entities

			DataManager::getInstance().applyStep(*manager, current_simulation_step);

			current_simulation_step++;
		}

		if (interpolation >= manager->steps.back().timestamp + 1.0f) {
			if (autoInterpolate) {
				interpolation = manager->steps.begin()->timestamp;
			}
			else {
				interpolation_running = false;
				interpolation = manager->steps.back().timestamp + 1.0f;
			}

			current_simulation_step = 0;
		}
	}

	//for all nodes and for all links, get interpolation and accordingly modify the animators?
	//if (interpolation_running) {

	//	for (NodeEntity* node_entity : config.scene->manager->getVisibleGroup<NodeEntity>(Manager::groupNodes_0)) {
	//		if (node_entity->hasComponent<Rectangle_w_Color>()) {
	//			node_entity->GetComponent<Rectangle_w_Color>().flash_animation.interpolation_a = interpolation;
	//			node_entity->GetComponent<Rectangle_w_Color>().setFlashFrame();
	//		}
	//	}

	//	for (LinkEntity* link_entity : config.scene->manager->getVisibleGroup<LinkEntity>(Manager::groupLinks_0)) {
	//		if (link_entity->hasComponent<Line_w_Color>()) {
	//			link_entity->GetComponent<Line_w_Color>().flash_animation.interpolation_a = interpolation;
	//			link_entity->GetComponent<Line_w_Color>().setFlashFrame();
	//		}
	//	}

	//}
}

void GraphTopBar::OnImGuiRender()
{

	std::vector<std::string> openTabs;
	for (const auto& [name, manager] : config.scene->managers) {
		openTabs.push_back(name);
	}

	float childHeight = 30.0f;
	tabToClose = "";

	ImGui::BeginChild("Scene Tabs", ImVec2(0, childHeight), true, ImGuiWindowFlags_NoScrollbar);


	if (ImGui::BeginTabBar("SceneTabs", ImGuiTabBarFlags_AutoSelectNewTabs)) {
		for (size_t i = 0; i < openTabs.size(); ++i) {
			std::string name = openTabs[i];

			bool open = true;

			if (ImGui::BeginTabItem(name.c_str(), &open, ImGuiTabItemFlags_None)) {
				if (!config.scene->managerName.empty()) {
					config.scene->managerName = name;
				}
				ImGui::EndTabItem();
			}
			if (!open) {
				tabToClose = name;

				// If we're closing the currently active tab, switch to another one
				if (!config.scene->managerName.empty() && config.scene->managerName.compare(name) == 0 && config.scene->managers.size() > 1) {
					// Find a different tab to make active
					for (const std::string& tabName : openTabs) {
						if (tabName != name) {
							config.scene->managerName = tabName;
							break;
						}
					}
				}
				else if (!config.scene->managerName.empty() && config.scene->managers.size() == 1) {
					// If this is the last tab, clear the current active
					config.scene->managerName.clear();
				}
			}
		}
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - 16);
		if (ImGui::ImageButton("play", static_cast<ImTextureID>(static_cast<intptr_t>(TextureManager::getInstance().Get_GLTexture("play-button")->id)), ImVec2(16, 16))) {
			interpolation_running = !interpolation_running;
		}

		ImGui::EndTabBar();
	}
	ImGui::EndChild();
	ImGui::BeginChild("Interpolation Slider", ImVec2(0, 40), true);
	{
		Manager* manager = config.scene->manager;

		ImGui::Text("Interpolation");
		ImGui::SameLine();
		ImGui::SliderFloat("##interp", &interpolation,
			manager->steps.size() ?
			manager->steps.begin()->timestamp : 0.0f,
			manager->steps.size() ?
			(manager->steps.back().timestamp + 1.0f) : 0.0f, "%.2f");

		// Optional: Add tooltip
		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::Text("Control interpolation");
			ImGui::EndTooltip();
		}
		ImGui::SameLine();
		ImGui::Text("Speed");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100.0f);
		ImGui::SliderFloat("##interp_speed", &interpolation_speed, 0.01f, 1.0f, "%.2f");
		ImGui::SameLine();
		ImGui::Checkbox("Auto", &autoInterpolate);

	}

	ImGui::EndChild();
}


std::string GraphTopBar::getTabToClose() {
	return tabToClose;
}