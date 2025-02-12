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

inline float pointLineDistance(glm::vec2 point, glm::vec2 lineStartPoint, glm::vec2 lineEndPoint) {
    float num = std::abs((lineEndPoint.y - lineStartPoint.y) * point.x - (lineEndPoint.x - lineStartPoint.x) * point.y + lineEndPoint.x * lineStartPoint.y - lineEndPoint.y * lineStartPoint.x);
    float den = std::sqrt((lineEndPoint.y - lineStartPoint.y) * (lineEndPoint.y - lineStartPoint.y) + (lineEndPoint.x - lineStartPoint.x) * (lineEndPoint.x - lineStartPoint.x));
    return num / den;
}


inline bool checkCircleLineCollision(glm::vec2 center, int circleRadius, glm::vec2 lineStartPoint, glm::vec2 lineEndPoint) {
    // Find the distance from the center of the circle to the line
    float dist = pointLineDistance(center, lineStartPoint, lineEndPoint);

    // Check if the distance is less than or equal to the radius of the circle
    if (dist <= circleRadius) {
        // Further check if the circle's center projection is within the line segment
        float dx = lineEndPoint.x - lineStartPoint.x;
        float dy = lineEndPoint.y - lineStartPoint.y;
        float t = ((center.x - lineStartPoint.x) * dx + (center.y - lineStartPoint.y) * dy) / (dx * dx + dy * dy);

        // Clamp t to the range [0, 1]
        t = std::max(0.0f, std::min(1.0f, t));

        // Find the closest point on the line to the circle center
        float closestX = lineStartPoint.x + t * dx;
        float closestY = lineStartPoint.y + t * dy;

        // Check if this closest point is within the circle
        float distanceToCircle = std::sqrt((closestX - center.x) * (closestX - center.x) +
            (closestY - center.y) * (closestY - center.y));
        return distanceToCircle <= circleRadius;
    }

    return false;
}