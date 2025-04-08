#pragma once

#include <map>
#include "Animation.h"
#include "MovingAnimation.h"
#include "FlashAnimation.h"


struct AnimatorManager
{
public:
	std::map<std::string, Animation> animations;
	std::map<std::string, MovingAnimation> moving_animations;
	std::map<std::string, FlashAnimation> flash_animations;

	AnimatorManager()
	{}

	static AnimatorManager& getInstance() {
		static AnimatorManager instance;
		return instance;
	}

	void InitializeAnimators()
	{
		Animation defaultAnimation = Animation(6, 2, 1, 0.04f, "looped");

		animations.emplace("Default", defaultAnimation);

		MovingAnimation defaultMoveAnimation = MovingAnimation(0, 0, 0, 0.0, "looped", 0, 0);
		
		moving_animations.emplace("Default", defaultMoveAnimation);

		FlashAnimation defaultFlashAnimation = FlashAnimation(0, 0, 3, 0.0f, "looped", { 0.2f, 1.0f, 0.2f, 1.0f }, { 255,255,255,255 });
		FlashAnimation lineTransferFlashAnimation = FlashAnimation(0, 0, 3, 0.01f, "play_n_times", { 0.01f, 0.01f, 0.00f, 0.01f }, { 255,255,255,255 }, 1);
		FlashAnimation rectangleInterpolationFlashAnimation = FlashAnimation(0, 0, 3, 0.01f, "back_forth", { 0.01f, 0.01f, 1.00f, 0.01f }, { 255,0,0,255 }, 0);

		flash_animations.emplace("Default", defaultFlashAnimation);
		flash_animations.emplace("LineTransfer", lineTransferFlashAnimation);
		flash_animations.emplace("RectInterpolation", rectangleInterpolationFlashAnimation);


	}
};
