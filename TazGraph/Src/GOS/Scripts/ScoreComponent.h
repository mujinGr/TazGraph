#pragma once

#include "GOS/Components.h"

extern Manager manager;

class ScoreComponent : public Component
{
private:
	int score;
	Entity* scorelabel;
public:
	ScoreComponent()
	{
		score = 0;
	}
	ScoreComponent(int scr)
	{
		score = scr;
	}
	~ScoreComponent()
	{}

	void init() override
	{
		scorelabel = &manager.addEntity(true);
		scorelabel->addComponent<TransformComponent>(glm::vec2(32, 576), Manager::actionLayer, glm::ivec2( 32, 32), 1);
		scorelabel->addComponent<UILabel>(&manager, "graph 0", "arial");
		scorelabel->addGroup(Manager::groupLabels);
	}

	void update(float deltaTime) override
	{

	}

	void addToScore(int scr)
	{
		score += scr;
		scorelabel->GetComponent<UILabel>().setLetters("graph" + std::to_string(score));
	}

	int getScore()
	{
		return score;
	}
};