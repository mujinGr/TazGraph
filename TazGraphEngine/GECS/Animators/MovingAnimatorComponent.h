#pragma once


#include "../Components.h"
#include <map>
#include "Animation.h"
#include "AnimatorManager.h"
// TODO: in comparison to AnimatorComponent, here we also have access to sprite so create functions like SetDx,SetDy,SetRepsOfMove,SetDelayOfMove, IsForeverRepeatedMove

class MovingAnimatorComponent : public AnimatorComponent //Animator -> Sprite -> Transform 
{					//! also we use MovingAnimator instead of simple Animator so that entities use less memory and we use it to entities that have triggers that change their animation
public:
	//std::map<const char*, Animation> animations; //Animator Manager

	MovingAnimatorComponent() : AnimatorComponent()
	{

	}

	MovingAnimatorComponent(std::string id) : AnimatorComponent(id)
	{

	}

	~MovingAnimatorComponent()
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
		if (sprite->moving_animation.hasFinished()) { // playing again animation
			sprite->moving_animation.finished = false;
			sprite->moving_animation.times_played = 0;
			resetAnimation();
		}

		sprite->transform->setPosition_X(sprite->transform->getPosition().x); //make player move with the camera, being stable in centre, except on edges
		sprite->transform->setPosition_Y(sprite->transform->getPosition().y);

		sprite->moving_animation.advanceFrame(deltaTime);
		if (sprite->moving_animation.positions.size() == 1) {
			sprite->setMoveFrame();
		}
		else {
			sprite->setSpecificMoveFrame();
		}
	}

	void draw(size_t e_index, PlaneModelRenderer& batch, TazGraphEngine::Window& window) override
	{
		//sprite->draw();
	}

	void Play(const char* animName, int reps = 0)
	{
		AnimatorManager& animManager = AnimatorManager::getInstance();
		animationName = animName;
		sprite->SetMovingAnimation(
			animManager.moving_animations[animationName].indexX, animManager.moving_animations[animationName].indexY,
			animManager.moving_animations[animationName].total_frames, animManager.moving_animations[animationName].speed,
			animManager.moving_animations[animationName].type,
			animManager.moving_animations[animationName].positions, animManager.moving_animations[animationName].zIndices, animManager.moving_animations[animationName].rotations, // here needs to be vector
			reps ? reps : animManager.moving_animations[animationName].reps
		);
	}

	void resetAnimation() {
		AnimatorManager& animManager = AnimatorManager::getInstance();
		animationName = "Default";
		sprite->SetMovingAnimation(
			animManager.moving_animations[animationName].indexX, animManager.moving_animations[animationName].indexY,
			animManager.moving_animations[animationName].total_frames, animManager.moving_animations[animationName].speed,
			animManager.moving_animations[animationName].type,
			animManager.moving_animations[animationName].positions, animManager.moving_animations[animationName].zIndices, animManager.moving_animations[animationName].rotations);
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