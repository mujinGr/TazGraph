#pragma once


#include "../../../UIElement.h"

struct NewMapConfig {

};

class NewMapUI : public UIElement
{
private:
	NewMapConfig config;

public:
	int newNodesCount = 0;
	int newLinksCount = 0;

	void setConfig(const NewMapConfig& cfg) { config = cfg; }
	void OnImGuiRender() override;
};