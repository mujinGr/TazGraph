#pragma once

#include <map>
#include "Animation.h"
#include "MovingAnimation.h"
#include "FlashAnimation.h"


struct AnimatorManager
{
public:
	std::map<const char*, Animation> animations;
	std::map<const char*, MovingAnimation> moving_animations;
	std::map<const char*, FlashAnimation> flash_animations;

	AnimatorManager()
	{}

	static AnimatorManager& getInstance() {
		static AnimatorManager instance;
		return instance;
	}

	void InitializeAnimators()
	{
		Animation defaultAnimation = Animation(6, 2, 1, 0.04, "looped");

		animations.emplace("Default", defaultAnimation);

		MovingAnimation defaultMoveAnimation = MovingAnimation(0, 0, 0, 0.0, "looped", 0, 0);
		
		moving_animations.emplace("Default", defaultMoveAnimation);

		FlashAnimation defaultFlashAnimation = FlashAnimation(0, 0, 4, 0, "looped", { 0.2f, 1.0f, 0.2f, 1.0f }, { 255,255,255,255 });

		flash_animations.emplace("Default", defaultFlashAnimation);

	}
};
