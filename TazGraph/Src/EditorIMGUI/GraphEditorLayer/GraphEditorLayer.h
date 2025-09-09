#pragma once


#include "../UIElement.h"

#include "../Components/MenuDropdown/MenuDropdown.h"
#include "../Components/GraphLeftPanel/GraphLeftPanel.h"
#include "../Components/GraphMiddlePanel/GraphMiddlePanel.h"
#include "../Components/GraphRightPanel/GraphRightPanel.h"


struct GraphEditorLayerConfig {

};

class GraphEditorLayer : public UIElement
{
private:
	GraphEditorLayerConfig  config;
public:
	GraphEditorLayer() {
		addUIComponent<MenuDropdownPanel>();
		addUIComponent<GraphLeftPanel>();
		addUIComponent<GraphMiddlePanel>();
		addUIComponent<GraphRightPanel>();
	}

	void setConfig(const GraphEditorLayerConfig& cfg) { config = cfg; }
	void OnImGuiRender() override;
};