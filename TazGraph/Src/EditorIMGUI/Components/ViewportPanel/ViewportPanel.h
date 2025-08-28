#pragma once


#include "../../UIElement.h"

#include "../Minimap/Minimap.h"
#include "../OrientationBox/OrientationBox.h"
#include <Renderers/FrameBuffer/Framebuffer.h>

struct ViewportPanelConfig {
	Framebuffer* c_fb;
	Framebuffer* c_minimap_fb;
	ImVec2* c_storedWindowPos; 
	ImVec2* c_storedWindowSize;
};

class ViewportPanel : public UIElement
{
private:
	Minimap _minimap;
	OrientationBox _orientationBox;


	ViewportPanelConfig config;

public:
	bool isMouseInSecondColumn = false;

	void update(float deltaTime) override;
	void setConfig(const ViewportPanelConfig& cfg) { config = cfg; }
	void OnImGuiRender() override;
	void updateIsMouseInSecondColumn();
};