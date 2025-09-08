#pragma once


#include "../../../UIElement.h"

struct VisibleEntitiesConfig {
	Manager* manager;
};

class VisibleEntitiesPanel : public UIElement
{
private:
	VisibleEntitiesConfig config;
public:
	void setConfig(const VisibleEntitiesConfig& cfg) { config = cfg; }
	void update(float deltaTime) override;
	void OnImGuiRender() override;
};