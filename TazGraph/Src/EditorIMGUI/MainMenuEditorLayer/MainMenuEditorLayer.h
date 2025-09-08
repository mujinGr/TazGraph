#pragma once

#include "../UIElement.h"
#include "../Components/MenuSceneControllers/LoadingUI/LoadingUI.h"
#include "../Components/MenuSceneControllers/NewMapUI/NewMapUI.h"

struct MainMenuEditorLayerConfig {
	std::function<void()> onStartClicked;
	std::function<void()> onExitClicked;
};

class MainMenuEditorLayer : public UIElement
{
private:
	MainMenuEditorLayerConfig config;
public:
	MainMenuEditorLayer() {
		addUIComponent<LoadingUI>();
		addUIComponent<NewMapUI>();
	}

	void setConfig(const MainMenuEditorLayerConfig& cfg) { config = cfg; }
	void update(float deltaTime) override;
	void OnImGuiRender() override;
};