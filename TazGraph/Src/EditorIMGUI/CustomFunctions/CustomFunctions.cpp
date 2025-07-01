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
		case 2:
			CalculateSignals();
			break;
		case 3:
			CalculateHeatMap();
			break;
		case 4:
			DrawCandlestickChart();
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

void CustomFunctions::CalculateSignals()
{
	if (ImPlot::BeginPlot("Dummy Signals")) {

		// Generate dummy sine and cosine signals
		static float x[1000], y1[1000], y2[1000];
		static bool initialized = false;
		if (!initialized) {
			for (int i = 0; i < 1000; ++i) {
				x[i] = i * 0.01f;
				y1[i] = sinf(x[i] * 2.0f * 3.14159f);  // Sine wave
				y2[i] = cosf(x[i] * 2.0f * 3.14159f);  // Cosine wave
			}
			initialized = true;
		}

		ImPlot::PlotLine("Sine", x, y1, 1000);
		ImPlot::PlotLine("Cosine", x, y2, 1000);

		ImPlot::EndPlot();
	}

	ImGui::End();
}

void CustomFunctions::CalculateHeatMap()
{
	if (ImPlot::BeginPlot("Simple Heatmap", ImVec2(-1, 300))) {

		static float values[50 * 50];
		static bool heatmapInitialized = false;

		if (!heatmapInitialized) {
			for (int y = 0; y < 50; ++y) {
				for (int x = 0; x < 50; ++x) {
					values[y * 50 + x] = (float)(x + y);  // Simple gradient pattern
				}
			}
			heatmapInitialized = true;
		}

		ImPlot::PlotHeatmap("Heatmap", values, 50, 50);

		ImPlot::EndPlot();
	}
}

void CustomFunctions::DrawCandlestickChart()
{
	if (ImPlot::BeginPlot("Candlestick", ImVec2(-1, 400))) {

		static const int numPoints = 100;
		static float xs[numPoints];
		static float open[numPoints];
		static float close[numPoints];
		static float low[numPoints];
		static float high[numPoints];
		static bool initialized = false;

		if (!initialized) {
			srand(42); // Consistent dummy data
			float price = 100.0f;
			for (int i = 0; i < numPoints; ++i) {
				xs[i] = (float)i;
				float delta = ((rand() % 200) - 100) * 0.01f;
				open[i] = price;
				close[i] = price + delta;
				low[i] = fminf(open[i], close[i]) - ((rand() % 100) * 0.01f);
				high[i] = fmaxf(open[i], close[i]) + ((rand() % 100) * 0.01f);
				price = close[i];
			}
			initialized = true;
		}

		// Draw candlesticks manually
		for (int i = 0; i < numPoints; ++i) {

			float x = xs[i];
			float bodyMin = fminf(open[i], close[i]);
			float bodyMax = fmaxf(open[i], close[i]);

			// Wick (high-low)
			ImPlot::PlotLine("##wicks", &x, &low[i], 1);
			ImPlot::PlotLine("##wicks", &x, &high[i], 1);

			// Body (open-close)
			float halfWidth = 0.3f;
			float barX[2] = { x - halfWidth, x + halfWidth };
			float barY[2] = { open[i], close[i] };

			ImPlot::PlotLine("##body", barX, barY, 2);
		}

		ImPlot::EndPlot();
	}

	ImGui::End();
}