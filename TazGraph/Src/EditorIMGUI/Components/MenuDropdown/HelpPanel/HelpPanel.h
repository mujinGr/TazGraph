#pragma once


#include "../../../UIElement.h"

struct HelpConfig {

};

class HelpPanel : public UIElement
{
private:
	HelpConfig  config;

	bool firstTime = true;
	void RenderCameraControls();
	void RenderKeyboardShortcuts();
public:
	bool showHelpPanel = false;

	void setConfig(const HelpConfig& cfg) { config = cfg; }
	void update(float deltaTime) override;
	void OnImGuiRender() override;
};