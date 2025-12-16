#pragma once

#include "../../../UtilComponents.h"

class SimDumpData : public Component //transform as in graphics, we have rotation and scale
{
public:
	std::string string_id = "";


	SimDumpData()
	{
	}

	SimDumpData(std::string exampleString)
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
		return "SimDumpData";
	}

	void showGUI(std::vector<BaseComponent*> otherComponents = {}) override {
		ImGui::Separator();


	};
};