#pragma once


#include "../../Components.h"
#include <map>
#include "../Animation.h"
#include "../AnimatorManager.h"


typedef uint32_t timestamp;


class LineFlashAnimatorComponent : public LinkComponent //Animator -> Sprite -> Transform 
{					//! also we use MovingAnimator instead of simple Animator so that entities use less memory and we use it to entities that have triggers that change their animation
public:

	Line_w_Color* line = nullptr;
	const char* animationName = NULL;
	timestamp resumeTime = 0;

	LineFlashAnimatorComponent()
	{

	}

	~LineFlashAnimatorComponent()
	{

	}

	void init() override
	{
		if (!entity->hasComponent<Line_w_Color>())
		{
			entity->addComponent<Line_w_Color>();
		}
		line = &entity->GetComponent<Line_w_Color>();

		Play("Default");
	}

	void update(float deltaTime) override
	{
		if (animationName == "Default") {
			return;
		}
		if (line->flash_animation.hasFinished()) { // playing again animation
			line->flash_animation.finished = false;
			line->flash_animation.times_played = 0;
			resetAnimation();
		}

		line->flash_animation.advanceFrame(deltaTime);
		line->setFlashFrame();
	}

	void draw(PlaneModelRenderer& batch, TazGraphEngine::Window& window) override
	{
		//sprite->draw(batch);
	}

	void Play(const char* animName, int reps = 0)
	{
		AnimatorManager& animManager = AnimatorManager::getInstance();
		animationName = animName;
		line->SetFlashAnimation(
			animManager.flash_animations[animationName].indexX, animManager.flash_animations[animationName].indexY,
			animManager.flash_animations[animationName].total_frames, animManager.flash_animations[animationName].speed,
			animManager.flash_animations[animationName].type,
			animManager.flash_animations[animationName].getSpeedsAsVector(),
			animManager.flash_animations[animationName].flashColor,
			reps ? reps : animManager.flash_animations[animationName].reps
		);
	}

	void resetAnimation() {
		AnimatorManager& animManager = AnimatorManager::getInstance();
		animationName = "Default";
		line->SetFlashAnimation(
			animManager.flash_animations[animationName].indexX, animManager.flash_animations[animationName].indexY,
			animManager.flash_animations[animationName].total_frames, animManager.flash_animations[animationName].speed,
			animManager.flash_animations[animationName].type,
			animManager.flash_animations[animationName].getSpeedsAsVector(),
			animManager.flash_animations[animationName].flashColor);
	}

	const char* getPlayName()
	{
		return animationName;
	}

};