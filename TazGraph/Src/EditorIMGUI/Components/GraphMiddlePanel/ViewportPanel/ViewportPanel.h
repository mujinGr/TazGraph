#pragma once


#include "../../../UIElement.h"

#include "Minimap/Minimap.h"
#include "OrientationBox/OrientationBox.h"
#include "HighlightBox/HighlightBox.h"
#include "SceneControl/SceneControl.h"
#include "HoverEntityPanel/HoverEntityPanel.h"
#include "../../EntityComponentsControl/EntityComponentsControl.h"


struct ViewportPanelConfig {
	Framebuffer* c_fb;
	Framebuffer* c_minimap_fb;
	glm::vec2 startPos;
	glm::vec2 currPos;
};

class ViewportPanel : public UIElement
{
private:
	ViewportPanelConfig config;

public:

	ViewportPanel() {

	}

	ViewportPanel(bool usePython) {
		addUIComponent<Minimap>();
		addUIComponent<OrientationBox>();
		addUIComponent<SceneControlPanel>();
		addUIComponent<HoverEntityPanel>();
		addUIComponent<HighlightBox>();
		addUIComponent<EntityComponentsControlPanel>(usePython);
	}

	bool isMouseInSecondColumn = false;

	void setConfig(const ViewportPanelConfig& cfg) { config = cfg; }
	void OnImGuiRender() override;
	void updateIsMouseInSecondColumn();
};