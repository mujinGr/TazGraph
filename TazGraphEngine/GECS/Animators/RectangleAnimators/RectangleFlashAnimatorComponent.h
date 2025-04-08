#pragma once


#include "../../Components.h"
#include <map>
#include "../Animation.h"
#include "../AnimatorManager.h"


typedef uint32_t timestamp;


class RectangleFlashAnimatorComponent : public Component //Animator -> Sprite -> Transform 
{					//! also we use MovingAnimator instead of simple Animator so that entities use less memory and we use it to entities that have triggers that change their animation
public:

	Rectangle_w_Color* rectangle = nullptr;
	std::string animationName = "";
	timestamp resumeTime = 0;

	RectangleFlashAnimatorComponent()
	{

	}

	~RectangleFlashAnimatorComponent()
	{

	}

	void init() override
	{
		if (!entity->hasComponent<Rectangle_w_Color>())
		{
			entity->addComponent<Rectangle_w_Color>();
		}
		rectangle = &entity->GetComponent<Rectangle_w_Color>();

		Play("Default");
	}

	void update(float deltaTime) override
	{
		if (animationName == "Default") {
			return;
		}
		if (rectangle->flash_animation.hasFinished()) { // playing again animation
			rectangle->flash_animation.finished = false;
			rectangle->flash_animation.times_played = 0;
			resetAnimation();
		}

		rectangle->flash_animation.advanceFrame(deltaTime);
		rectangle->setFlashFrame();
	}

	void draw(size_t e_index, PlaneModelRenderer& batch, TazGraphEngine::Window& window) override
	{
		//sprite->draw(batch);
	}

	void Play(const std::string& animName, int reps = 0)
	{
		AnimatorManager& animManager = AnimatorManager::getInstance();
		animationName = animName;
		FlashAnimation& flash_animation = animManager.flash_animations[animationName];
		rectangle->SetFlashAnimation(
			flash_animation.indexX, flash_animation.indexY,
			flash_animation.total_frames, flash_animation.speed,
			flash_animation.type,
			flash_animation.getSpeedsAsVector(),
			flash_animation.flashColor,
			reps ? reps : flash_animation.reps
		);
	}

	void resetAnimation() {
		AnimatorManager& animManager = AnimatorManager::getInstance();
		animationName = "Default";
		rectangle->SetFlashAnimation(
			animManager.flash_animations[animationName].indexX, animManager.flash_animations[animationName].indexY,
			animManager.flash_animations[animationName].total_frames, animManager.flash_animations[animationName].speed,
			animManager.flash_animations[animationName].type,
			animManager.flash_animations[animationName].getSpeedsAsVector(),
			animManager.flash_animations[animationName].flashColor);
	}

	std::string getPlayName()
	{
		return animationName;
	}

};