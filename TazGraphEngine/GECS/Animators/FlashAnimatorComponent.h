#pragma once


#include "../Components.h"
#include <map>
#include "Animation.h"
#include "AnimatorManager.h"

class FlashAnimatorComponent : public AnimatorComponent //Animator -> Sprite -> Transform 
{					//! also we use MovingAnimator instead of simple Animator so that entities use less memory and we use it to entities that have triggers that change their animation
public:
	//std::map<const char*, Animation> animations; //Animator Manager

	FlashAnimatorComponent() : AnimatorComponent()
	{

	}

	FlashAnimatorComponent(std::string id) : AnimatorComponent(id)
	{

	}

	~FlashAnimatorComponent()
	{

	}

	void init() override
	{
		if (!entity->hasComponent<SpriteComponent>())
		{
			entity->addComponent<SpriteComponent>(textureid);
		}
		sprite = &entity->GetComponent<SpriteComponent>();

		Play("Default");
		sprite->setTex(textureid);
	}

	void update(float deltaTime) override
	{
		if (sprite->flash_animation.hasFinished()) { // playing again animation
			sprite->flash_animation.finished = false;
			sprite->flash_animation.times_played = 0;
			resetAnimation();
		}

		sprite->flash_animation.advanceFrame(deltaTime);
		sprite->setFlashFrame();
	}

	void draw(size_t e_index, PlaneModelRenderer&  batch, TazGraphEngine::Window& window) override
	{
		//sprite->draw(batch);
	}

	void Play(std::string animName, int reps = 0)
	{
		AnimatorManager& animManager = AnimatorManager::getInstance();
		animationName = animName;
		sprite->SetFlashAnimation(
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
		sprite->SetFlashAnimation(
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

	void DestroyTex()
	{
		sprite->DestroyTex();
	}

};