#pragma once

#include "../../UIElement.h"

#include "./CustomFunctions/CustomFunctions.h"
#include "../../../AssetManager/AssetManager.h"

struct GraphRightConfig {
	Manager* c_manager;
	float* c_nodeRadius;
	std::vector<std::pair<Entity*, glm::vec3>> c_selectedEntities;
};

class GraphRightPanel : public UIElement
{
private:
	GraphRightConfig config;
public:

	GraphRightPanel() {
		addUIComponent<CustomFunctions>();
	}

	void setConfig(const GraphRightConfig& cfg) { config = cfg; }

	void update(float deltaTime) override;

	void OnImGuiRender() override;

	void availableFunctions();
	void ShowAllEntities();
	template<typename TVec>
	static void DrawEntityJumpList(const char* labelId, TVec& vec);

	template<typename EntityType>
	void DrawBulkComponentControls(const std::vector<EntityType*>& entityVec, const std::string& componentCategory, const std::string& uniqueID);
	void CopyComponentValues(BaseComponent* source, BaseComponent* target, size_t size);
};