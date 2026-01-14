#pragma once

#include "../../../Components.h"

// public Component need to be first for scanning
class EmptyPythonCodeComponent : public Component, public PythonCodeComponent//transform as in graphics, we have rotation and scale
{
public:

	EmptyPythonCodeComponent()
	{
	}


	void init() override
	{
		PythonCodeComponent::init();
	}
	void update(float deltaTime) override
	{
		PythonCodeComponent::update(deltaTime);


	}

	std::string GetComponentName() override {
		return "EmptyPythonCodeComponent";
	}

	void showGUI(std::vector<BaseComponent*> otherComponents = {}) override {
		if (ImGui::CollapsingHeader("Script"))
		{
			PythonCodeComponent::showGUI();


		}

	};

	void showGUI(std::vector<BaseComponent*> otherComponents, std::vector<Entity*> otherEntities) override {
		ImGui::TextDisabled("Modify Script Components Seperately");

	}
};