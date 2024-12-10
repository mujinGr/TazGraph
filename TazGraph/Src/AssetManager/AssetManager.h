#pragma once

#include <string>
#include "TextureManager/TextureManager.h"
#include "../ShapeManager.h"
#include "ECS\ECSManager.h"
#include <TTF/SDL_ttf.h>
#include "../Graph.h"


class AssetManager //this class created when we added projectiles, based on this class other components changed
{					//it just replaces the paths of textures with names
public:

	SDL_Color black = { 0, 0 ,0 ,255 };
	SDL_Color white = { 255, 255 ,255 ,255 };
	SDL_Color red = { 255, 0 ,0 ,255 };
	SDL_Color green = { 0, 255 ,0 ,255 };

	AssetManager(Manager* man, InputManager& inputManager, TazGraphEngine::Window& window);
	~AssetManager();

	//gameobjects
	void CreatePlayer(Entity& player);

private:
	Manager* manager;
	InputManager& _inputManager;
	TazGraphEngine::Window& _window;
};