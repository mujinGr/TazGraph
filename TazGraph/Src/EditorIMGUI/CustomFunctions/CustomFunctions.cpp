#include "CustomFunctions.h"
#include <unordered_set>

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
	std::vector<int> plotOutLinks(4, 0);
	std::unordered_set<NodeEntity*> currentDepthNodes;
	std::unordered_set<NodeEntity*> nextDepthNodes;

	// Add selected entities (only nodes) as depth 0
	for (auto& pair : m_selectedEntities) {
		if (auto* node = dynamic_cast<NodeEntity*>(pair.first)) {
			currentDepthNodes.insert(node);
		}
	}

	// Traverse depths 0 to 3
	for (int depth = 0; depth < 4; depth++) {
		int outlinkCount = 0;

		// Collect next depth nodes
		for (auto* node : currentDepthNodes) {
			for (auto* link : node->getOutLinks()) {
				if (auto* target = link->getToNode()) {
					nextDepthNodes.insert(target);
				}
			}
			outlinkCount += node->getOutLinks().size();
		}

		plotOutLinks[depth] = outlinkCount;

		// Move to the next depth level
		currentDepthNodes = std::move(nextDepthNodes);
		nextDepthNodes.clear();
	}


	const char* depthLabels[] = { "Depth 0", "Depth 1", "Depth 2", "Depth 3" };
	float xValues[] = { 0, 1, 2, 3 };

	if (ImPlot::BeginPlot("Outlinks Per Depth")) {
		ImPlot::SetupAxes("Depth", "Outlinks", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
		ImPlot::PlotBars("Outlinks", plotOutLinks.data(), 4, 0.5f);
		ImPlot::EndPlot();
	}
}
