#pragma once


#include "../../UIElement.h"

#include "ViewportPanel/ViewportPanel.h"
#include "GraphTopBar/GraphTopBar.h"
#include "../PythonInterpreterPanel/PythonInterpreterPanel.h"

struct GraphMiddlePanelConfig {
	IScene* scene;
	Framebuffer* c_framebuffer;
	Framebuffer* c_minimapFramebuffer;
	ImVec2* c_viewportPos;
	ImVec2* c_viewportSize;
	glm::vec2 startPos;
	glm::vec2 currPos;
};

class GraphMiddlePanel : public UIElement
{
public:
	GraphMiddlePanelConfig  config;

	GraphMiddlePanel() {
		addUIComponent<GraphTopBar>();
		addUIComponent<ViewportPanel>();
		//addUIComponent<PythonInterpreterPanel>();
	}

	void setConfig(const GraphMiddlePanelConfig& cfg) { config = cfg; }
	void OnImGuiRender() override;
};