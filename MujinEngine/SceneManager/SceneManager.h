#pragma once

#include <string>
#include "../TextureManager/TextureManager.h"
#include "../ECS\ECSManager.h"

class SceneManager //this class created when we added projectiles, based on this class other components changed
{					//it just replaces the paths of textures with names
public:
	SceneManager();
	~SceneManager();

	glm::ivec2 GetSceneStartupPosition(int i);
	SDL_Rect GetSceneCamera(int i);
	void AddSceneCamera(SDL_Rect scene);
private:
	std::vector<glm::ivec2> transitionScenePositions;
	std::vector<SDL_Rect> sceneCameras;
};