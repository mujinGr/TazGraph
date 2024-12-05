#pragma once

#include <string>
#include "TextureManager/TextureManager.h"
#include "../ShapeManager.h"
#include "ECS\ECSManager.h"
#include <TTF/SDL_ttf.h>
#include "../Game.h"


class AssetManager //this class created when we added projectiles, based on this class other components changed
{					//it just replaces the paths of textures with names
public:

	SDL_Color black = { 0, 0 ,0 ,255 };
	SDL_Color white = { 255, 255 ,255 ,255 };
	SDL_Color red = { 255, 0 ,0 ,255 };
	SDL_Color green = { 0, 255 ,0 ,255 };

	AssetManager(Manager* man, InputManager& inputManager, MujinEngine::Window& window);
	~AssetManager();

	//gameobjects
	void CreatePlayer(Entity& player);

	//triggers
	bool OnPipeTrigger(SDL_Rect collider) const;
	bool LeftOfPipeTrigger(SDL_Rect collider) const;

private:
	Manager* manager;
	InputManager& _inputManager;
	MujinEngine::Window& _window;

	SDL_Rect onpipeTriggers[2];
	SDL_Rect leftofpipeTriggers[1];
};