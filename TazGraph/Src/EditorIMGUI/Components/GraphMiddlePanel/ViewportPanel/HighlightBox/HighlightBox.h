#pragma once


#include "../../../../UIElement.h"

struct HighlightBoxConfig {
	glm::vec2 startPos;
	glm::vec2 currPos;
};

class HighlightBox : public UIElement
{
private:
	HighlightBoxConfig  config;
public:
	void setConfig(const HighlightBoxConfig& cfg) { config = cfg; }
	void OnImGuiRender() override;
};