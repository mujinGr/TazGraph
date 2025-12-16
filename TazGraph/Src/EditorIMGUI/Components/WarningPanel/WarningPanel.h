#pragma once


#include "../../UIElement.h"

struct WarningConfig {

};

class WarningPanel : public UIElement
{
private:
	WarningConfig config;
public:
	void setConfig(const WarningConfig& cfg) { config = cfg; }
	void update(float deltaTime) override;
	void OnImGuiRender() override;
};