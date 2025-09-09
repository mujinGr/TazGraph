#pragma once


#include "../../UIElement.h"

#include "ViewportPanel/ViewportPanel.h"
#include "GraphTopBar/GraphTopBar.h"

struct GraphMiddlePanelConfig {
	IScene* scene;
	std::function<void(std::string m_managerName)> setManager;
	Framebuffer* c_framebuffer;
	Framebuffer* c_minimapFramebuffer;
	ImVec2* c_windowPos;
	ImVec2* c_windowSize;
};

class GraphMiddlePanel : public UIElement
{
public:
	GraphMiddlePanelConfig  config;

	GraphMiddlePanel() {
		addUIComponent<GraphTopBar>();
		addUIComponent<ViewportPanel>();
	}

	void setConfig(const GraphMiddlePanelConfig& cfg) { config = cfg; }
	void OnImGuiRender() override;
};