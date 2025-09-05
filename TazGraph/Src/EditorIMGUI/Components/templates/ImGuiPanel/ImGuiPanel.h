#pragma once


#include "../../../UIElement.h"

struct ImGuiConfig {

};

class ImGuiPanel : public UIElement
{
private:
	ImGuiConfig  config;
public:
	void setConfig(const ImGuiConfig& cfg) { config = cfg; }
	void update(float deltaTime) override;
	void OnImGuiRender() override;
};