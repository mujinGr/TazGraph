#include "GraphTopBar.h"

void GraphTopBar::update(float deltaTime) {
	UIElement::update(deltaTime);

	if (!config.scene) return; // not yet initialized by scene
	Manager* manager = config.scene->manager;

	int& current_simulation_step = manager->currentStep;


	if (!manager->steps.empty()) {
		if (interpolation_running) {
			interpolation += interpolation_speed * deltaTime / config.scene->getApp()->getFPSLimiter().fps;
		}

		// here, get interpolation and manager steps intervals
		int targetStep = -1;
		int i = 0;
		for (auto it = manager->steps.begin(); it != manager->steps.end(); ++it, ++i)
		{
			auto nextIt = std::next(it);

			if (interpolation >= it->timestamp)
			{
				// Last step
				if (nextIt == manager->steps.end())
				{
					targetStep = i;
					interpolation = it->timestamp; // clamp
				}
				// Between this step and the next one
				else if (interpolation < nextIt->timestamp)
				{
					targetStep = i;
				}
			}
		}

		// Apply the step if found and different from current
		if (targetStep != -1 && current_simulation_step != targetStep) {
			current_simulation_step = targetStep;
			DataManager::getInstance().applyStep(*manager, current_simulation_step);
		}

		if (interpolation >= manager->steps.back().timestamp) {
			if (autoInterpolate) {
				interpolation = manager->steps.begin()->timestamp;
			}
			else {
				interpolation_running = false;
				interpolation = manager->steps.back().timestamp;
			}

		}
	}

}

void GraphTopBar::OnImGuiRender()
{

	std::vector<std::string> openTabs;
	for (const auto& [name, manager] : config.scene->managers) {
		openTabs.push_back(name);
	}

	float childHeight = 30.0f;
	tabToClose = "";

	bool childActive = ImGui::BeginChild("Scene Tabs", ImVec2(0, childHeight), true, ImGuiWindowFlags_NoScrollbar);

	if (childActive) {
		if (ImGui::BeginTabBar("SceneTabs", ImGuiTabBarFlags_AutoSelectNewTabs)) {
			for (size_t i = 0; i < openTabs.size(); ++i) {
				std::string name = openTabs[i];

				bool open = true;

				if (ImGui::BeginTabItem(name.c_str(), &open, ImGuiTabItemFlags_None)) {
					if (name.c_str() != DataManager::getInstance().getMapToLoad()) {
						DataManager::getInstance().setMapToLoad(name);
					}
					ImGui::EndTabItem();
				}
				if (!open) {
					tabToClose = name;

					// If we're closing the currently active tab, switch to another one
					if (!DataManager::getInstance().getMapToLoad().empty() &&
						DataManager::getInstance().getMapToLoad().compare(name) == 0 &&
						config.scene->managers.size() > 1)
					{
						// Find a different tab to make active
						for (const std::string& tabName : openTabs) {
							if (tabName != name) {
								DataManager::getInstance().setMapToLoad(tabName);
								break;
							}
						}
					}
					else if (!DataManager::getInstance().getMapToLoad().empty() &&
						config.scene->managers.size() == 1) {
						// If this is the last tab, clear the current active
						DataManager::getInstance().setMapToLoad("");
					}
				}
			}
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 16);
			const char* textureName = interpolation_running ? "pause-button" : "play-button";
			if (ImGui::ImageButton("play",
				static_cast<ImTextureID>(static_cast<intptr_t>(TextureManager::getInstance().Get_GLTexture(textureName)->id)),
				ImVec2(16, 16))) {
				interpolation_running = !interpolation_running;
			}
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip(interpolation_running ? "Stop Simulation" : "Start Simulation");
			}

			ImGui::EndTabBar();
		}
	}
	ImGui::EndChild();//? Needs to be outside


	childActive = ImGui::BeginChild("Interpolation Slider", ImVec2(0, 60), true);
	if (childActive)
	{
		Manager* manager = config.scene->manager;

		ImGui::Text("Time");
		ImGui::SameLine();
		float temp_value = static_cast<float>(interpolation);
		if (ImGui::SliderFloat("##interp", &temp_value,
			manager->steps.size() ?
			manager->steps.begin()->timestamp : 0.0f,
			manager->steps.size() ?
			(manager->steps.back().timestamp) : 0.0f, "%.10f"))
		{
			interpolation = static_cast<double>(temp_value);
		}
		ImGui::SameLine();
		ImGui::Text("(%d / %zu)", manager->currentStep, manager->steps.size() - 1);

		// Manual time input
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		if (ImGui::InputDouble("Set Time", &interpolation))
			interpolation_running = false;
		// Optional: Add tooltip
		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::Text("Control interpolation");
			ImGui::EndTooltip();
		}
		ImGui::SameLine();
		bool hasSteps = manager->steps.size() > 0;

		// Previous step button
		if (ImGui::ArrowButton("##prev_step", ImGuiDir_Left)) {
			if (hasSteps && manager->currentStep > 0) {
				manager->currentStep--;

				auto stepIt = manager->steps.begin();
				std::advance(stepIt, manager->currentStep);

				interpolation = stepIt->timestamp;
				DataManager::getInstance().applyStep(*manager, manager->currentStep);
				interpolation_running = false;
			}
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Previous Step");
		}
		ImGui::SameLine();

		// Next step button
		if (ImGui::ArrowButton("##next_step", ImGuiDir_Right)) {
			if (hasSteps && manager->currentStep < manager->steps.size() - 1) {
				manager->currentStep++;

				auto stepIt = manager->steps.begin();
				std::advance(stepIt, manager->currentStep);

				interpolation = stepIt->timestamp;
				DataManager::getInstance().applyStep(*manager, manager->currentStep);
				interpolation_running = false;
			}
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Next Step");
		}
		ImGui::SameLine();

		ImGui::Checkbox("Auto-Replay", &autoInterpolate);

		ImGui::Text("Simulation Speed");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100.0f);
		float speed_temp_value = static_cast<float>(interpolation_speed);
		if (ImGui::SliderFloat("##interp_speed", &speed_temp_value, 1e-9, 1.0f, "%.10f"))
		{
			interpolation_speed = static_cast<double>(speed_temp_value);
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		ImGui::InputDouble("Set Speed", &interpolation_speed);

	}
	ImGui::EndChild();//? Needs to be outside
}


std::string GraphTopBar::getTabToClose() {
	return tabToClose;
}