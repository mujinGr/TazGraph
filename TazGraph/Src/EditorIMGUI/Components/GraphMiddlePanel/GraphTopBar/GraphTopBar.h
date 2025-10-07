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

	bool inited = false;

public:
	void update(float deltaTime) override;
	void setConfig(const GraphTopConfig& cfg) {
		config = cfg;
		if (!inited) {
			interpolation =
				cfg.scene->manager->steps.empty() ?
				0.0f : cfg.scene->manager->steps.begin()->timestamp;
			inited = true;
		}
	}
	void OnImGuiRender() override;
	std::string getTabToClose();
};