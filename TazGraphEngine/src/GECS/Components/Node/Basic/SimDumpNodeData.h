#pragma once

#include "../../../Components.h"

class SimDumpNodeData : public Component //transform as in graphics, we have rotation and scale
{
public:
	std::string string_id = "";


	SimDumpNodeData()
	{
	}

	SimDumpNodeData(std::string exampleString)
	{
		string_id = exampleString;
	}


	void init() override
	{
	}
	void update(float deltaTime) override
	{



	}

	std::string GetComponentName() override {
		return "SimDumpNodeData";
	}

	void showGUI(std::vector<BaseComponent*> otherComponents = {}) override {
		ImGui::Separator();


	};
};