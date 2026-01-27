#pragma once

#include "../UIElement.h"
#include "../Components/MenuSceneControllers/LoadingUI/LoadingUI.h"
#include "../Components/MenuSceneControllers/NewMapUI/NewMapUI.h"

struct MainMenuEditorLayerConfig {
	std::function<void()> onStartClicked;
	std::function<void()> onExitClicked;
	Framebuffer* viewportFramebuffer = nullptr;

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
	void OnImGuiRender() override;
};