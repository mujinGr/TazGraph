#pragma once


#include "../Components.h"
#include <map>
#include "Animation.h"
#include "AnimatorManager.h"

class MovingAnimatorComponent : public Component //Animator -> Sprite -> Transform 
{					//! also we use MovingAnimator instead of simple Animator so that entities use less memory and we use it to entities that have triggers that change their animation
public:
	//std::map<const char*, Animation> animations; //Animator Manager
	TransformComponent* transform = nullptr;
	std::string animationName = "";

	MovingAnimatorComponent()
	{

	}

	~MovingAnimatorComponent()
	{

	}

	void init() override
	{
		if (!entity->hasComponent<TransformComponent>())
		{
			entity->addComponent<TransformComponent>();
		}
		transform = &entity->GetComponent<TransformComponent>();

		Play("Default");
	}

	void update(float deltaTime) override
	{
		if (animationName == "Default") return;

		if (transform->moving_animation.hasFinished()) { // playing again animation
			transform->moving_animation.finished = false;
			transform->moving_animation.times_played = 0;
			resetAnimation();
		}

		transform->moving_animation.advanceFrame(deltaTime);


		transform->setMoveFrame();

	}

	void draw(size_t e_index, PlaneModelRenderer& batch, TazGraphEngine::Window& window) override
	{
		//sprite->draw();
	}

	void Play(const char* animName, int reps = 0)
	{
		AnimatorManager& animManager = AnimatorManager::getInstance();
		animationName = animName;
		transform->SetMovingAnimation(
			transform->getPosition(),
			animManager.moving_animations[animationName].total_frames, animManager.moving_animations[animationName].speed,
			animManager.moving_animations[animationName].type,
			animManager.moving_animations[animationName].dest_position, animManager.moving_animations[animationName].dest_rotation, // here needs to be vector
			reps ? reps : animManager.moving_animations[animationName].reps
		);
	}

	void Play(const char* animName, glm::vec3 m_dest_posistion, int reps = 0)
	{
		AnimatorManager& animManager = AnimatorManager::getInstance();
		animationName = animName;
		transform->SetMovingAnimation(

			transform->getPosition(),

			animManager.moving_animations[animationName].total_frames, animManager.moving_animations[animationName].speed,
			animManager.moving_animations[animationName].type,
			m_dest_posistion, animManager.moving_animations[animationName].dest_rotation, // here needs to be vector
			reps ? reps : animManager.moving_animations[animationName].reps
		);
	}

	void resetAnimation() {
		transform->moving_animation.resetFrameIndex();

		AnimatorManager& animManager = AnimatorManager::getInstance();
		animationName = "Default";
		transform->SetMovingAnimation(
			transform->getPosition(),
			animManager.moving_animations[animationName].total_frames, animManager.moving_animations[animationName].speed,
			animManager.moving_animations[animationName].type,
			animManager.moving_animations[animationName].dest_position, animManager.moving_animations[animationName].dest_rotation);
	}

	std::string getPlayName()
	{
		return animationName;
	}

};