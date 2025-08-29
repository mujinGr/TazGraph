#pragma once


#include "../../UIElement.h"

struct LoadingConfig {

};

class LoadingPanel : public UIElement
{
private:
	LoadingConfig config;

public:
	void setConfig(const LoadingConfig& cfg) { config = cfg; }
	void OnImGuiRender() override;
};