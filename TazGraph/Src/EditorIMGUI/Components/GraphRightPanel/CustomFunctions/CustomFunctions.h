#pragma once

#include "../../../UIElement.h"

struct CustomFunctionsConfig {
	IScene* scene;
};


class CustomFunctions : public UIElement
{
private:
	std::vector<std::pair<EntityID, glm::vec3>>* selectedEntities = nullptr;
	CustomFunctionsConfig config;

public:

	void OnImGuiRender() override;

	bool isScriptResultsOpen = false;
	int activatedScriptShown = 0;

	void default_renderUI();

	void CalculateDegree();
	void CalculateSignals();
	void CalculateHeatMap();
	void DrawCandlestickChart();

	void setSelectedEntities(std::vector<std::pair<EntityID, glm::vec3 >>& m_selectedEntities);
	void setConfig(const CustomFunctionsConfig& cfg) { config = cfg; }
};