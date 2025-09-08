#pragma once

#include "../../../UIElement.h"
#include <AppScene/AppInterface.h>

struct GraphTopConfig {
	BaseFPSLimiter* c_fpsLimiter;
	std::unordered_map<std::string, Manager*>* c_graphNames = {
	};
	std::string* c_currentActive;
	Manager* c_manager;
};

class GraphTopBar : public UIElement
{
private:
	bool interpolation_running = false;

	float interpolation = 0.0f;
	float interpolation_speed = 0.01f;

	std::string currentActive;

	GraphTopConfig config;
	std::string tabToClose = "";

public:
	void update(float deltaTime) override;
	void setConfig(const GraphTopConfig& cfg) { config = cfg; }
	void OnImGuiRender() override;
	std::string getTabToClose();
};