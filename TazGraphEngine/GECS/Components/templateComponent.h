#pragma once

#include "../Components.h"

class templateComponent : public Component //transform as in graphics, we have rotation and scale
{
public:
	std::string last_position = "";


	templateComponent()
	{
	}


	void init() override
	{
	}
	void update(float deltaTime) override
	{



	}

	std::string GetComponentName() override {
		return "templateComponent";
	}

	void showGUI(std::vector<BaseComponent*> otherComponents = {}) override {
		ImGui::Separator();


	};
};