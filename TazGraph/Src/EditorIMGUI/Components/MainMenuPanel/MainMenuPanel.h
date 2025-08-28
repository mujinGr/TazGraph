#pragma once


#include "../../UIElement.h"
#include "../MenuSceneControllers/NewMapUI/NewMapUI.h"
#include "../MenuSceneControllers/LoadingUI/LoadingUI.h"

struct MainMenuConfig {
	std::function<void()> onStartClicked;
	std::function<void()> onExitClicked;
};

class MainMenuPanel : public UIElement
{
private:
	MainMenuConfig config;

public:
	LoadingUI loadingUI;
	NewMapUI newMapUI;
	void setConfig(const MainMenuConfig& cfg) { config = cfg; }
	void OnImGuiRender() override;
};