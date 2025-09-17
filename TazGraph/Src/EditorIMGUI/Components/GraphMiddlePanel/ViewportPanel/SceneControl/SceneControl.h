#pragma once


#include "../../../../UIElement.h"

struct SceneControlConfig {
	IScene* scene;
};

class SceneControlPanel : public UIElement
{
private:
	SceneControlConfig config;

public:
	glm::vec2 savedMousePos = glm::vec2(0);

	void setConfig(const SceneControlConfig& cfg) { config = cfg; }
	void OnImGuiRender() override;
};