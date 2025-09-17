#pragma once


#include "../UIElement.h"

#include "../Components/MenuDropdown/MenuDropdown.h"
#include "../Components/GraphLeftPanel/GraphLeftPanel.h"
#include "../Components/GraphMiddlePanel/GraphMiddlePanel.h"
#include "../Components/GraphRightPanel/GraphRightPanel.h"
#include <Renderers/FrameBuffer/Framebuffer.h>


struct GraphEditorLayerConfig {
	IScene* scene;
	glm::vec2 _viewportMousePosition;
	std::function<void(std::string m_managerName)> setManager;

	Framebuffer* viewportFramebuffer;
	Framebuffer* minimapFramebuffer;
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