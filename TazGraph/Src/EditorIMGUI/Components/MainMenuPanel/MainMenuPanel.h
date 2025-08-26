#pragma once


#include "../../UIElement.h"

struct MainMenuConfig {
	std::function<void()> onStartClicked;
	std::function<void()> onLoadClicked;
	std::function<void()> onExitClicked;
};

class MainMenuPanel : public UIElement
{
private:
	MainMenuConfig config;

public:
	void setConfig(const MainMenuConfig& cfg) { config = cfg; }
	void OnImGuiRender() override;
};