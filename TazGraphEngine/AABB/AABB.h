#pragma once
#include <SDL2/SDL_rect.h>
#include <type_traits>

template<typename RectType>
inline bool checkCollision(const RectType& recA, const RectType& recB) {
	static_assert(std::is_same<RectType, SDL_Rect>::value || std::is_same<RectType, SDL_FRect>::value,
		"checkCollision: RectType must be either SDL_Rect or SDL_FRect");

	if (recA.x >= recB.x + recB.w || recA.x + recA.w <= recB.x ||
		recA.y >= recB.y + recB.h || recA.y + recA.h <= recB.y) {
		return false; // no collision
	}
	return true;
}

inline bool checkCollision(const SDL_Rect& recA, const SDL_FRect& recB) {
	SDL_FRect convertedA = { static_cast<float>(recA.x), static_cast<float>(recA.y),
							static_cast<float>(recA.w), static_cast<float>(recA.h) };
	return checkCollision(convertedA, recB);
}

inline bool checkCollision(const SDL_FRect& recA, const SDL_Rect& recB) {
	return checkCollision(recB, recA); 
}