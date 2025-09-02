#pragma once


#include "../../UIElement.h"
#include "../MenuSceneControllers/NewMapUI/NewMapUI.h"
#include "../MenuSceneControllers/SavingUI/SavingUI.h"
#include "../MenuSceneControllers/LoadingUI/LoadingUI.h"
#include "../PythonInterpreterPanel/PythonInterpreterPanel.h"

struct MenuDropdownConfig {

};

class MenuDropdownPanel : public UIElement
{
private:
	MenuDropdownConfig config;
public:
	PythonInterpreterPanel pythonInterpreter;
	LoadingUI loadingUI;
	SavingUI savingUI;
	NewMapUI newMapUI;
	void setConfig(const MenuDropdownConfig& cfg) { config = cfg; }
	void update(float deltaTime) override;
	void OnImGuiRender() override;
};