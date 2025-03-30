#include "CustomFunctions.h"

void CustomFunctions::renderUI(Manager& manager, std::vector<std::pair<Entity*, glm::vec3>>& m_selectedEntities)
{

	ImVec2 initialPos = ImVec2(10, 10); // Top-left corner
	ImVec2 initialSize = ImVec2(400, 200); // Default size

	if (isScriptResultsOpen) {
		ImGui::SetNextWindowPos(initialPos, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(initialSize, ImGuiCond_FirstUseEver);

		ImGui::Begin("Script Results", &isScriptResultsOpen);
		switch (activatedScriptShown) {
		case 0:
			default_renderUI();
			break;
		case 1:
			CalculateDegree(manager, m_selectedEntities);
			break;
		}
		ImGui::End();
	}
}

void CustomFunctions::default_renderUI()
{
	ImGui::Text("No results to be shown");
}

void CustomFunctions::CalculateDegree(Manager& manager, std::vector<std::pair<Entity*, glm::vec3>>& m_selectedEntities)
{
	for (auto& ent : manager.getVisible<NodeEntity>()) {
		ImGui::Text("asdasd");
	}
}
