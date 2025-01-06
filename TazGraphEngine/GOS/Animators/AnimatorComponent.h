#pragma once

#include "../Components.h"
#include <map>
#include "Animation.h"
#include "AnimatorManager.h"
#include <functional>

typedef uint32_t timestamp;


class AnimatorComponent : public Component //Animator -> Sprite -> Transform
{
public:

	// onAction is the same thing as Play()
	// onFinish and onStart are for when we free Animator and when we initialize it
	// difference between this and from the lectures is that in lectures it uses seperate animator for each animation

	SpriteComponent* sprite = nullptr;
	std::string textureid;
	const char* animationName = NULL;
	timestamp resumeTime = 0;

	//std::map<const char*, Animation> animations; //Animator Manager

	AnimatorComponent()
	{

	}

	AnimatorComponent(std::string id)
	{
		textureid = id;
	}

	~AnimatorComponent()
	{

	}

	void init() override
	{
		if (!entity->hasComponent<SpriteComponent>())
		{
			entity->addComponent<SpriteComponent>(textureid);
		}
		sprite = &entity->GetComponent<SpriteComponent>();

		Play("P1Idle"); //onStart
		sprite->setTex(textureid);
	}

	void update(float deltaTime) override //onAction
	{
		if (sprite->animation.hasFinished()) { // playing again animation
			sprite->animation.finished = false;
			sprite->animation.times_played = 0;
			resetAnimation();
		}

		sprite->animation.advanceFrame(deltaTime);
		sprite->setCurrFrame();
	}

	void draw(PlaneModelRenderer&  batch, TazGraphEngine::Window& window) override
	{
		//sprite->draw(batch);
	}

	void Play(const char* animName, int reps = 0)
	{
		AnimatorManager& animManager = AnimatorManager::getInstance();
		animationName = animName;
		sprite->SetAnimation(animManager.animations[animName].indexX, animManager.animations[animName].indexY,
			animManager.animations[animName].total_frames, animManager.animations[animName].speed,
			animManager.animations[animName].type, 
			reps ? reps : animManager.animations[animName].reps );
	}

	void resetAnimation() {
		AnimatorManager& animManager = AnimatorManager::getInstance();
		animationName = "P1Idle";
		sprite->SetAnimation(
			animManager.animations[animationName].indexX, animManager.animations[animationName].indexY,
			animManager.animations[animationName].total_frames, animManager.animations[animationName].speed,
			animManager.animations[animationName].type
		);
	}

	const char* getPlayName()
	{
		return animationName;
	}

	void DestroyTex()
	{
		sprite->DestroyTex();
	}

};