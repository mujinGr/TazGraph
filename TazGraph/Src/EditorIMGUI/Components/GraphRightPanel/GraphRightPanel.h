#pragma once

#include "../../UIElement.h"

#include "./CustomFunctions/CustomFunctions.h"
#include "../../../AssetManager/AssetManager.h"
#include "./EntityComponentsControl/EntityComponentsControl.h"

struct GraphRightConfig {
	IScene* scene;
	std::vector<SelectedInfo> c_selectedEntities;
};

class GraphRightPanel : public UIElement
{
private:
	GraphRightConfig config;
	int selectedGroup = -1;
public:

	GraphRightPanel(bool usePython) {
		addUIComponent<CustomFunctions>();
		addUIComponent<EntityComponentsControlPanel>(usePython);
	}

	void setConfig(const GraphRightConfig& cfg) { config = cfg; }

	void update(float deltaTime) override;

	void OnImGuiRender() override;

	void availableFunctions();
	void ShowGroupComponents();

	void DrawBulkComponentControls(const std::vector<Entity*>& entityVec, const std::string& componentCategory, const std::string& uniqueID);
	void CopyComponentValues(BaseComponent* source, BaseComponent* target, size_t size);
};