#pragma once

#include "../../UIElement.h"

struct EntityComponentsConfig {
	glm::vec2 mousePos; 
	Entity* displayedEntity; 
	Manager* manager;
};

class EntityComponentsControlPanel : public UIElement
{
private:
	EntityComponentsConfig config;

	int _lastEntityDisplayed = 0;

public:
	void setConfig(const EntityComponentsConfig& cfg) { config = cfg; }
	void OnImGuiRender() override;
	void StartPollingComponent(Entity* entity, const std::string& fileName);
};