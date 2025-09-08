#pragma once

#include "../../UIElement.h"

#include "../../../AssetManager/AssetManager.h"

struct GraphLeftConfig {
	bool* renderDebug;
	glm::vec2 sceneMouseCoords;
	glm::vec2 mouseCoords;
	Manager* manager = nullptr;
};

class GraphLeftPanel : public UIElement
{
private:
	bool _clusterLayout = false;
	GraphLeftConfig config;

	int last_activeLayout = 0;
	int activeLayout = 0;
public:
	void setConfig(const GraphLeftConfig& cfg) { config = cfg; }

	void update(float deltaTime) override;

	void OnImGuiRender() override;
};