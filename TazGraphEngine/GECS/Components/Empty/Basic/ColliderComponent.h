#pragma once
#include <string>
#include <SDL2/SDL.h>
#include "../../../Components.h"
#include "../../../../TextureManager/TextureManager.h"

#define COL_POS_OFFSET 8

class ColliderComponent : public Component //collider -> transform
{
public:
	SDL_FRect collider{ 0,0,0,0 };

	SDL_FRect srcR{ 0,0,0,0 }, destR{0,0,0,0};

	TransformComponent* transform = nullptr;

	ColliderComponent()
	{
	}

	// todo instead have offset
	ColliderComponent(float xpos, float ypos, float size)
	{
		collider.x = xpos;
		collider.y = ypos;
		collider.h = collider.w = size;
	}

	ColliderComponent(SDL_FRect colliderRect)
	{
		collider = colliderRect;
	}

	void init() override
	{
		if (!entity->hasComponent<TransformComponent>()) //todo: problem: having transform on top left grid, not every collider its own
		{
			entity->addComponent<TransformComponent>();
		}
		transform = &entity->GetComponent<TransformComponent>();

		srcR = { 0, 0, 8, 8 };
		destR = { collider.x, collider.y , collider.w , collider.h };

		//Graph::colliders.push_back(this);

	}

	void update(float deltaTime) override
	{
		collider.x = (transform->getPosition().x);
		collider.y = (transform->getPosition().y);
		collider.w = transform->size.x * transform->scale;
		collider.h = transform->size.y * transform->scale;

		//destR.x = collider.x - main_camera2D->worldLocation.x;
		//destR.y = collider.y - main_camera2D->worldLocation.y;
	}

	void draw(size_t e_index, PlaneModelRenderer&  batch, TazGraphEngine::Window& window) override
	{
	}

	SDL_FRect getRect()
	{
		return collider;
	}

	std::string GetComponentName() override {
		return "ColliderComponent";
	}
};