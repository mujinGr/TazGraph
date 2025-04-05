#pragma once

#include <string>
#include "TextureManager/TextureManager.h"
#include "GECS/Core/GECSManager.h"
#include <SDL2/SDL_ttf.h>
#include "../Graph.h"

struct PairHash {
	template <class T1, class T2>
	std::size_t operator()(const std::pair<T1, T2>& p) const {
		auto hash1 = std::hash<T1>{}(p.first);
		auto hash2 = std::hash<T2>{}(p.second);
		return hash1 ^ (hash2 << 1);
	}
};
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
	void CreateWorldMap(Entity& worldMap);
	void CreateGroup(Entity& groupNode, glm::vec3 centerGroup, float groupNodeSize, Grid::Level m_level);

	void CreateGroupLink(Entity& groupLink, Grid::Level m_level);

	void createGroupLayout(Grid::Level m_level);

	void ungroupLayout(Grid::Level m_level);

private:
	Manager* manager;
	InputManager& _inputManager;
	TazGraphEngine::Window& _window;
};