#pragma once


#include "../UIElement.h"

#include "../Components/MenuDropdown/MenuDropdown.h"
#include "../Components/GraphLeftPanel/GraphLeftPanel.h"
#include "../Components/GraphMiddlePanel/GraphMiddlePanel.h"
#include "../Components/GraphRightPanel/GraphRightPanel.h"
#include <Renderers/FrameBuffer/Framebuffer.h>
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

	std::vector<std::pair<Entity*, glm::vec3>>* selectedEntities;

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
	bool leftColumnExpanded = true;
	bool rightColumnExpanded = true;

	GraphEditorLayer() {
		addUIComponent<MenuDropdownPanel>();
		addUIComponent<GraphLeftPanel>();
		addUIComponent<GraphMiddlePanel>();
		addUIComponent<GraphRightPanel>();
	}

	void setConfig(const GraphEditorLayerConfig& cfg) { config = cfg; }
	void OnImGuiRender() override;
};