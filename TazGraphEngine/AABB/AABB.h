#pragma once
#include <SDL2/SDL_rect.h>

static inline bool checkCollision(const SDL_FRect recA, const SDL_FRect recB) {
	if (recA.x >= recB.x + recB.w || recA.x + recA.w <= recB.x ||
		recA.y >= recB.y + recB.h || recA.y + recA.h <= recB.y) {
		return false; // no collision
	}
	return true;
}