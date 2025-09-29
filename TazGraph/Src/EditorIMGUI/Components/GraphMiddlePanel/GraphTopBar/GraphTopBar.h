#pragma once

#include "../../../UIElement.h"

struct GraphTopConfig {
	IScene* scene;
};

class GraphTopBar : public UIElement
{
private:
	bool interpolation_running = false;

	float interpolation = 0.0f;
	float interpolation_speed = 0.01f;
	
	bool autoInterpolate = false;

	std::string currentActive;

	GraphTopConfig config;
	std::string tabToClose = "";


	int current_simulation_step = 0;

public:
	void update(float deltaTime) override;
	void setConfig(const GraphTopConfig& cfg) { config = cfg; }
	void OnImGuiRender() override;
	std::string getTabToClose();
};