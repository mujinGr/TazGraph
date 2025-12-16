#pragma once


#include "../UIElement.h"

#include "../Components/MenuDropdown/MenuDropdown.h"
#include "../Components/GraphLeftPanel/GraphLeftPanel.h"
#include "../Components/GraphMiddlePanel/GraphMiddlePanel.h"
#include "../Components/GraphRightPanel/GraphRightPanel.h"
#include "../../Map/Map.h"



struct GraphEditorLayerConfig {
	IScene* scene;
	glm::vec2 viewportMousePosition;

	Framebuffer* viewportFramebuffer = nullptr;
	Framebuffer* minimapFramebuffer = nullptr;

	ImVec2* viewportPos;
	ImVec2* viewportSize;

	glm::vec2 selectionWindowStartPos = glm::vec2(0);
	glm::vec2 selectionWindowCurrentPos = glm::vec2(0);

	std::vector<SelectedInfo>* selectedEntities;

	Map* map = nullptr;

	Entity* onHoverEntity = nullptr;
	Entity* displayedEntity = nullptr;

	bool sceneManagerActive = false;

};

class GraphEditorLayer : public UIElement
{
private:
	GraphEditorLayerConfig  config;
public:

	enum COL_STATE {
		COLLAPSED,
		EXPANDED
	};

	COL_STATE leftColumnState = COL_STATE::EXPANDED;
	COL_STATE last_leftColumnState = COL_STATE::EXPANDED;

	COL_STATE rightColumnState = COL_STATE::EXPANDED;
	COL_STATE last_rightColumnState = COL_STATE::EXPANDED;

	GraphEditorLayer() {

	}

	GraphEditorLayer(bool usePython) {
		addUIComponent<MenuDropdownPanel>();
		addUIComponent<GraphLeftPanel>();
		addUIComponent<GraphMiddlePanel>(usePython);
		addUIComponent<GraphRightPanel>();
	}

	void setConfig(const GraphEditorLayerConfig& cfg) { config = cfg; }
	void OnImGuiRender() override;
};