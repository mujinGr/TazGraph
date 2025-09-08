#pragma once


#include "../../../../UIElement.h"
#include "../../../MenuSceneControllers/NewMapUI/NewMapUI.h"
#include "../../../MenuSceneControllers/SavingUI/SavingUI.h"
#include "../../../MenuSceneControllers/LoadingUI/LoadingUI.h"

struct HoverEntityConfig {
	glm::vec2 mousePos;
	Entity* hoveredEntity;
	Manager* manager;
};

class HoverEntityPanel : public UIElement
{
private:
	HoverEntityConfig config;

public:
	void setConfig(const HoverEntityConfig& cfg) { config = cfg; }
	void OnImGuiRender() override;
};