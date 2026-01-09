#pragma once

#include "../../UIElement.h"

struct EntityGroupsConfig {
	IScene* scene;
	Entity* displayedEntity = nullptr;
};

class EntityGroupsControlPanel : public UIElement
{
private:
	EntityGroupsConfig config;

	EntityID _lastEntityDisplayed = 0;

public:
	EntityGroupsControlPanel() {
	}

	void setConfig(const EntityGroupsConfig& cfg) { config = cfg; }
	void OnImGuiRender() override;
};