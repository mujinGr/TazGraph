#pragma once


#include "../../../UIElement.h"
#include "../../../../Map/Map.h"

struct SavingUIConfig {
	Map* c_map;
};

class SavingUI : public UIElement
{
private:
	SavingUIConfig config;

public:
	void setConfig(const SavingUIConfig& cfg) { config = cfg; }
	void OnImGuiRender() override;
};