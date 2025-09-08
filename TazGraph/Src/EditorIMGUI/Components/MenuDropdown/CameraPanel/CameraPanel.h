#pragma once


#include "../../../UIElement.h"
#include "SliderRotateZ.h"

struct CameraPanelConfig {

};

class CameraPanel : public UIElement
{
private:
	int _currentOrientationIndex = 0;
	CameraPanelConfig   config;
public:
	CameraPanel() {
		addUIComponent<SliderRotateZ>();
	}
	void setConfig(const CameraPanelConfig& cfg) { config = cfg; }
	void update(float deltaTime) override;
	void OnImGuiRender() override;
};