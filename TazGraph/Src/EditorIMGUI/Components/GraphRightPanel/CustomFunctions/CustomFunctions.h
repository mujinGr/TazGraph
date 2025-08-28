#pragma once

#include "../../../UIElement.h"
#include <GECS/Core/GECSEntity.h>

class CustomFunctions : public UIElement 
{
private:
	std::vector<std::pair<Entity*, glm::vec3>>* selectedEntities = nullptr;

public:

	void OnImGuiRender() override;

	bool isScriptResultsOpen = false;
	int activatedScriptShown = 0;
	
	void default_renderUI();

	void CalculateDegree();
	void CalculateSignals();
	void CalculateHeatMap();
	void DrawCandlestickChart();

	void setSelectedEntities(std::vector<std::pair<Entity*, glm::vec3 >> & m_selectedEntities);

};