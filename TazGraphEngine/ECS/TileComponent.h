#pragma once

#include <string>
#include <SDL/SDL.h>
#include "Components.h"

class TileComponent : public Component //tileComp --> GridComp --> ColliderComp -> transform
{
public:
	bool fullSolid;
	bool hasGrid = false;

	SDL_Rect destRect;
	glm::ivec2 position;

	TransformComponent* transform = nullptr;
	int scaledTile;

	TileComponent() = default;

	~TileComponent()
	{
	}

	TileComponent(int xpos, int ypos,int tsize, int tscale)
	{
		position.x = xpos;
		position.y = ypos;

		destRect.x = xpos; //destRect. x/y may not be needed
		destRect.y = ypos;
		destRect.w = destRect.h = tsize * tscale; //tsize * tscale = 32

		scaledTile = tsize * tscale;
	}
	
	void init() override
	{
		if (!entity->hasComponent<TransformComponent>())
		{
			entity->addComponent<TransformComponent>(static_cast<glm::vec2>(position) , Manager::actionLayer, glm::ivec2(destRect.w, destRect.h), 1);
			entity->GetComponent<TransformComponent>().setVelocity_X(0);
			entity->GetComponent<TransformComponent>().setVelocity_Y(0);
		}
		transform = &entity->GetComponent<TransformComponent>();

		// make this rectangle_w_Color before turning it into circle
		
		entity->addComponent<Rectangle_w_Color>();
		entity->GetComponent<Rectangle_w_Color>().color = Color(255, 255, 255, 255);
	}
	
	void update(float deltaTime) override //function like in sprite, but diffent moving in draw
	{
		
	}

	void draw(SpriteBatch&  batch, TazGraphEngine::Window& window) override
	{
		//sprite->draw(batch);
		//TextureManager::Draw(texture, srcRect, destRect, SDL_FLIP_NONE);
	}
};