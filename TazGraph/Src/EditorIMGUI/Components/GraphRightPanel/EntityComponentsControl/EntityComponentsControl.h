#pragma once

#include "../../../UIElement.h"
#include "../../PythonInterpreterPanel/PythonInterpreterPanel.h"

struct EntityComponentsConfig {
	IScene* scene;
	std::vector<EntityID> displayedEntityIds;
	int selectedGroup = -1;
};

class EntityComponentsControlPanel : public UIElement
{
private:
	EntityComponentsConfig config;

	std::vector<Entity*> displayedEntities;

public:
	EntityComponentsControlPanel(bool usePython) {
		if (usePython) {
			addUIComponent<PythonInterpreterPanel>();
		}
	}

	void setConfig(const EntityComponentsConfig& cfg) {
		config = cfg;
		displayedEntities = config.selectedGroup != -1 ?
			config.scene->manager->getGroup_All(config.selectedGroup)
			: config.scene->manager->getEntities_FromIds(config.displayedEntityIds);
	}

	void OnImGuiRender() override;
	void ComponentCheckbox(std::string c);
};