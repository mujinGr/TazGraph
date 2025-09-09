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
	bool showVisibleEntities = false;
	void setConfig(const VisibleEntitiesConfig& cfg) { config = cfg; }
	void OnImGuiRender() override;
};