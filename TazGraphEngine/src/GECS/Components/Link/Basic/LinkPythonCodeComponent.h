#pragma once

#include "../../../Components.h"

// public Component need to be first for scanning
class LinkPythonCodeComponent : public LinkComponent, public PythonCodeComponent //transform as in graphics, we have rotation and scale
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
		if (ImGui::CollapsingHeader("Script"))
		{
			PythonCodeComponent::showGUI();


		}


	};

	void showGUI(std::vector<BaseComponent*> otherComponents, std::vector<Entity*> otherEntities) override {
		ImGui::TextDisabled("Modify Script Components Seperately");

	}
};