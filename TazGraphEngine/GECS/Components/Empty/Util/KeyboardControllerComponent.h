#pragma once


#include "../../../Components.h"


#include "../../../../InputManager/InputManager.h"

constexpr float walkingSpeed = 3.5f, runningSpeed = 8.5f, jumpingSpeed = 3.0f;


class KeyboardControllerComponent : public Component //! moving animation
{
public: //TODO: maybe have variables as private
	InputManager* _inputManager = nullptr;

	TransformComponent* transform = nullptr;
	AnimatorComponent* animator = nullptr;
	RigidBodyComponent* rigidbody = nullptr;
	SpriteComponent* sprite = nullptr;

	SDL_KeyCode walkUpKey = SDLK_UNKNOWN, walkLeftKey = SDLK_UNKNOWN, walkRightKey = SDLK_UNKNOWN, walkDownKey = SDLK_UNKNOWN;


	KeyboardControllerComponent()
	{

	}

	KeyboardControllerComponent(
		InputManager* inputManager,
		SDL_KeyCode walkUpKey,
		SDL_KeyCode walkLeftKey,
		SDL_KeyCode walkRightKey,
		SDL_KeyCode walkDownKey
		)
		: _inputManager(inputManager),
		walkUpKey(walkUpKey),
		walkLeftKey(walkLeftKey),
		walkRightKey(walkRightKey),
		walkDownKey(walkDownKey)
	{

	}

	~KeyboardControllerComponent()
	{

	}

	void init() override
	{
		transform = &entity->GetComponent<TransformComponent>();
		animator = &entity->GetComponent<AnimatorComponent>();
		sprite = &entity->GetComponent<SpriteComponent>();
	}

	void update(float deltaTime) override
	{

		if (_inputManager->isKeyDown(walkLeftKey)) {
			transform->setVelocity_X(-runningSpeed);
		}
		if (_inputManager->isKeyDown(walkRightKey)) {
			transform->setVelocity_X(runningSpeed);
		}
		if (!_inputManager->isKeyDown(walkRightKey) && !_inputManager->isKeyDown(walkLeftKey)) {
			transform->setVelocity_X(0);
		}
		if (_inputManager->isKeyDown(walkUpKey)) {
			transform->setVelocity_Y(-runningSpeed);
		}
		if (_inputManager->isKeyDown(walkDownKey)) {
			transform->setVelocity_Y(runningSpeed);
		}
		if (!_inputManager->isKeyDown(walkUpKey) && !_inputManager->isKeyDown(walkDownKey)) {
			transform->setVelocity_Y(0);
		}
	}

	std::string GetComponentName() override {
		return "KeyboardControllerComponent";
	}
};