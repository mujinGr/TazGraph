#pragma once


#include "../../../UIElement.h"

#include "Minimap/Minimap.h"
#include "OrientationBox/OrientationBox.h"
#include "HighlightBox/HighlightBox.h"
#include "SceneControl/SceneControl.h"
#include "HoverEntityPanel/HoverEntityPanel.h"
#include "../../EntityGroupsControl/EntityGroupsControl.h"


struct ViewportPanelConfig {
	Framebuffer* c_fb;
	Framebuffer* c_minimap_fb;
	ImVec2* c_storedWindowPos;
	ImVec2* c_storedWindowSize;
	glm::vec2 startPos;
	glm::vec2 currPos;
};

class ViewportPanel : public UIElement
{
private:
	ViewportPanelConfig config;

public:

	ViewportPanel() {
		addUIComponent<Minimap>();
		addUIComponent<OrientationBox>();
		addUIComponent<SceneControlPanel>();
		addUIComponent<HoverEntityPanel>();
		addUIComponent<HighlightBox>();
		addUIComponent<EntityGroupsControlPanel>();
	}

	bool isMouseInSecondColumn = false;

	void setConfig(const ViewportPanelConfig& cfg) { config = cfg; }
	void OnImGuiRender() override;
	void updateIsMouseInSecondColumn();
};