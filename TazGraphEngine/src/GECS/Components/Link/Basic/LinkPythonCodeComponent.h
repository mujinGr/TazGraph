#pragma once

#include "../../../Components.h"


class LinkPythonCodeComponent : public PythonCodeComponent, public LinkComponent //transform as in graphics, we have rotation and scale
{
public:
	std::string code = "";


	LinkPythonCodeComponent()
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
		return "LinkPythonCodeComponent";
	}

	void showGUI(std::vector<BaseComponent*> otherComponents = {}) override {
		PythonCodeComponent::showGUI();


	};
};