#pragma once

#include "../Components.h"

class PythonCodeComponent //transform as in graphics, we have rotation and scale
{
public:
	std::string code = "";


	PythonCodeComponent()
	{
	}


	void init()
	{
	}
	void update(float deltaTime)
	{

	}

	std::string GetComponentName() {
		return "PythonCodeComponent";
	}

	void showGUI(std::vector<BaseComponent*> otherComponents = {}) {
		ImGui::Separator();


	};
};