#pragma once


#include "../../../UIElement.h"

struct ScenePanelConfig {
	IScene* scene;
};

class ScenePanel : public UIElement
{
private:
	ScenePanelConfig   config;
public:
	bool showScenePanel = false;
	ScenePanel() {
	}
	void setConfig(const ScenePanelConfig& cfg) { config = cfg; }
	void update(float deltaTime) override;
	void OnImGuiRender() override;
};
