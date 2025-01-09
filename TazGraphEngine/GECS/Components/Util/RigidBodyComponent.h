#pragma once
#include "../../Components.h"

class RigidBodyComponent : public Component
{
private:
	TransformComponent* transform = nullptr;
public:
	float GravityForce = 1.0f;
	float accelGravity = 0.045f;
	float maxGravity = 3.f;
	bool onGround = false;
	bool justjumped = false;
	
	RigidBodyComponent() = default;

	RigidBodyComponent(float acc, float maxg)
	{
		accelGravity = acc;
		maxGravity = maxg;
	}

	~RigidBodyComponent()
	{

	}

	void init() override
	{
		transform = &entity->GetComponent<TransformComponent>();
	}

	void update(float deltaTime) override
	{
		if (onGround && !justjumped)
		{
			GravityForce = 1.0f;
			transform->setVelocity_Y(static_cast<int>(GravityForce));
			GravityForce = 0.0f;
		}
		else
		{
			justjumped = false;
			GravityForce += accelGravity * deltaTime;
			transform->setVelocity_Y(transform->getVelocity().y + static_cast<int>(GravityForce) * deltaTime);
			if (transform->getVelocity().y > static_cast<int>(maxGravity))
			{
				transform->setVelocity_Y(static_cast<int>(maxGravity));
			}
		}
	}
};