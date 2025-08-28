#pragma once


#include "../../../UIElement.h"

struct LoadingUIConfig {

};

class LoadingUI : public UIElement
{
private:
	LoadingUIConfig config;

public:
	void setConfig(const LoadingUIConfig& cfg) { config = cfg; }
	void OnImGuiRender() override;
};