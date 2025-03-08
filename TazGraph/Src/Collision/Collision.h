#pragma once
#include <SDL2/SDL.h>
#include "GECS/Components.h"
#include "GECS/UtilComponents.h"
#include "Window/Window.h"
#include "AABB/AABB.h"
#include <algorithm>

class Collision;

class Collision
{
public:
	enum ColSide { NONE = 0, TOP = 1, RIGHT = 2, DOWN = 3, LEFT = 4 };

	ColSide movingRectColSide;

	SDL_Rect storedColliderRect;
	glm::ivec2 overlap;
	bool isSidewaysCollision = false;
	bool isCollision = false;

	bool checkCollisionIsSideways(const SDL_Rect& moving_recA, const SDL_Rect& recB);
	void moveFromCollision(EmptyEntity& player);
	bool moveFromOuterBounds();
};