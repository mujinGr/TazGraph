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

	EntityID _lastEntityDisplayed = 0;

public:
	EntityComponentsControlPanel() {
		addUIComponent<PythonInterpreterPanel>();
	}

	void setConfig(const EntityComponentsConfig& cfg) { config = cfg; }
	void OnImGuiRender() override;
	void ComponentCheckbox(std::string c);
};