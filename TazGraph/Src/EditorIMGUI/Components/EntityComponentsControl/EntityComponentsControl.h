#pragma once

#include "../../UIElement.h"
#include "../PythonInterpreterPanel/PythonInterpreterPanel.h"

struct EntityComponentsConfig {
	IScene* scene;
	Entity* displayedEntity = nullptr;
};

class EntityComponentsControlPanel : public UIElement
{
private:
	EntityComponentsConfig config;

	int _lastEntityDisplayed = 0;

public:
	EntityComponentsControlPanel() {
		addUIComponent<PythonInterpreterPanel>();
	}

	void setConfig(const EntityComponentsConfig& cfg) { config = cfg; }
	void OnImGuiRender() override;
	void StartPollingComponent(Entity* entity, const std::string& fileName);
	void ComponentCheckbox(std::string c);
};