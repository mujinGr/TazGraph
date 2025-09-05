#pragma once

#include "../../UIElement.h"

#include "../SliderRotateZ.h"
#include "./CustomFunctions/CustomFunctions.h"
#include "../../../AssetManager/AssetManager.h"

struct GraphRightConfig {
	Manager* c_manager;
	float* c_nodeRadius;
	std::vector<std::pair<Entity*, glm::vec3>> c_selectedEntities;
};

class GraphRightPanel : public UIElement
{
private:
	GraphRightConfig config;

	CustomFunctions _customFunctions;
public:
	void setConfig(const GraphRightConfig& cfg) { config = cfg; }

	void update(float deltaTime) override;

	void OnImGuiRender() override;

	void availableFunctions();
	void ShowAllEntities();
	template<typename TVec>
	static void DrawEntityJumpList(const char* labelId, TVec& vec);
};