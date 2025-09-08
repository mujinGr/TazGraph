#pragma once


#include "../../UIElement.h"

#include "ViewportPanel/ViewportPanel.h"
#include "GraphTopBar/GraphTopBar.h"

struct GraphMiddlePanelConfig {

};

class GraphMiddlePanel : public UIElement
{
private:
	GraphMiddlePanelConfig  config;
public:

	GraphMiddlePanel() {
		addUIComponent<GraphTopBar>();
		addUIComponent<ViewportPanel>();
	}

	void setConfig(const GraphMiddlePanelConfig& cfg) { config = cfg; }
	void update(float deltaTime) override;
	void OnImGuiRender() override;
};