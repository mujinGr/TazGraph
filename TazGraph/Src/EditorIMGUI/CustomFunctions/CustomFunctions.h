#pragma once

#include "GECS/Components.h"
#include "GECS/UtilComponents.h"

class CustomFunctions {
public:

	bool isScriptResultsOpen = false;
	int activatedScriptShown = 0;
	
	void renderUI(Manager& manager, std::vector<std::pair<Entity*, glm::vec3>>& m_selectedEntities);

	void default_renderUI();

	void CalculateDegree(Manager& manager, std::vector<std::pair<Entity*, glm::vec3>>& m_selectedEntities);
	void CalculateSignals();
	void CalculateHeatMap();
	void DrawCandlestickChart();
};