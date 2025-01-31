#pragma once

#include <string>
#include "TextureManager/TextureManager.h"
#include "GECS/GECSManager.h"
#include <SDL2/SDL_ttf.h>
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

	//graphobjects
	void CreateCursor(Entity& cursor);
	void CreateGroup(Entity& groupNode, SDL_FRect centerCell, float groupNodeSize);

	void CreateGroupLink(Entity& groupLink);

private:
	Manager* manager;
	InputManager& _inputManager;
	TazGraphEngine::Window& _window;
};