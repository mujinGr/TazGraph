#pragma once


#include "../../UIElement.h"
#include "../MenuSceneControllers/NewMapUI/NewMapUI.h"
#include "../MenuSceneControllers/SavingUI/SavingUI.h"
#include "../MenuSceneControllers/LoadingUI/LoadingUI.h"
#include "../PythonInterpreterPanel/PythonInterpreterPanel.h"

#include "CameraPanel/CameraPanel.h"
#include "VisibleEntities/VisibleEntities.h"
#include "FPSCounter.h"

struct MenuDropdownConfig {

};

class MenuDropdownPanel : public UIElement
{
private:
	MenuDropdownConfig config;
public:
	PythonInterpreterPanel pythonInterpreter;

	MenuDropdownPanel()
	{
		addUIComponent<LoadingUI>();
		addUIComponent<SavingUI>();
		addUIComponent<NewMapUI>();
		addUIComponent<CameraPanel>();
		addUIComponent<VisibleEntitiesPanel>();
		addUIComponent<FPSCounter>();
	}

	void setConfig(const MenuDropdownConfig& cfg) { config = cfg; }
	void update(float deltaTime) override;
	void OnImGuiRender() override;
};