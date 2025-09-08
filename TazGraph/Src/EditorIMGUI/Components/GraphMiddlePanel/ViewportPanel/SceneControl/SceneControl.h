#pragma once


#include "../../../../UIElement.h"

struct SceneControlConfig {
	glm::vec2 c_mouseCoords;
	Manager* c_manager;
};

class SceneControlPanel : public UIElement
{
private:
	SceneControlConfig config;

public:
	void setConfig(const SceneControlConfig& cfg) { config = cfg; }
	void OnImGuiRender() override;
};