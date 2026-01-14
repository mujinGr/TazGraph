#pragma once

#include "../../../Components.h"

class InnerLink : public LinkComponent //transform as in graphics, we have rotation and scale
{
public:
	EntityID first_pathLink = 0;
	EntityID second_pathLink = 0;

	InnerLink()
	{
	}

	InnerLink(
		EntityID first,
		EntityID second
	) :
		first_pathLink(first),
		second_pathLink(second)
	{

	}
	~InnerLink()
	{
	}

	void init() override
	{
	}
	void update(float deltaTime) override
	{

	}

	std::string GetComponentName() override {
		return "InnerLink";
	}

	void showGUI(std::vector<BaseComponent*> otherComponents = {}) override {
		ImGui::Separator();


	};

	void showGUI(std::vector<BaseComponent*> otherComponents, std::vector<Entity*> otherEntities) override {
		ImGui::Separator();
	};
};