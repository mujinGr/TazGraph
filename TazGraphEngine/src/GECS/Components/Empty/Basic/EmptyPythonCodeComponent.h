#pragma once

#include "../../../Components.h"


class EmptyPythonCodeComponent : public PythonCodeComponent, public Component //transform as in graphics, we have rotation and scale
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
		PythonCodeComponent::showGUI();


	};
};