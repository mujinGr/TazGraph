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

inline bool rayIntersectsRectangle(const glm::vec3& rayOrigin, const glm::vec3& rayDirection,
    const glm::vec3& planePoint, const glm::vec3& planeNormal,
    float xMin, float xMax, float yMin, float yMax) {
    // Step 1: Check if the ray intersects the plane
    float denom = glm::dot(planeNormal, rayDirection);

    // If denom == 0, the ray is parallel to the plane
    if (std::abs(denom) < 1e-6) {
        return false; // No intersection
    }

    // Calculate t
    float t = glm::dot(planeNormal, planePoint - rayOrigin) / denom;

    // If t < 0, the intersection is behind the ray origin
    if (t < 0) {
        return false;
    }

    // Calculate the intersection point
    glm::vec3 intersectionPoint = rayOrigin + t * rayDirection;

    // Step 2: Check if the intersection point lies within the rectangle bounds
    if (intersectionPoint.x >= xMin && intersectionPoint.x <= xMax &&
        intersectionPoint.y >= yMin && intersectionPoint.y <= yMax) {
        return true; // Intersection point is within the rectangle
    }

    return false; // Intersection point is outside the rectangle
}


inline bool rayIntersectsSphere(
    const glm::vec3& rayOrigin,
    const glm::vec3& rayDirection,
    const glm::vec3& sphereCenter,
    float radius,
    glm::vec3& t
) {
    glm::vec3 oc = rayOrigin - sphereCenter;

    float A = glm::dot(rayDirection, rayDirection);  // Always 1 since rayDirection is normalized
    float B = 2.0f * glm::dot(oc, rayDirection);
    float C = glm::dot(oc, oc) - radius * radius;

    float discriminant = B * B - 4 * A * C;

    if (discriminant < 0) {
        return false;  // No intersection
    }

    // Compute the closest valid t value
    float sqrtDiscriminant = sqrt(discriminant);
    float t0 = (-B - sqrtDiscriminant) / (2.0f * A);
    float t1 = (-B + sqrtDiscriminant) / (2.0f * A);

    if (t0 >= 0) {
        t = rayOrigin + t0 * rayDirection;  // Closest intersection point
        return true;
    }
    else if (t1 >= 0) {
        t = rayOrigin + t1 * rayDirection;  // Intersection behind the ray origin
        return true;
    }

    return false;
}


inline bool rayIntersectsLineSegment(
    const glm::vec3& rayOrigin,
    const glm::vec3& rayDirection,
    const glm::vec3& segmentStart,
    const glm::vec3& segmentEnd,
    glm::vec3& intersectionPoint
) {
    for (float t = 0.0f; t < 1000.0f; t += 5.0f) {  // Limit ray to reasonable range
        glm::vec3 samplePoint = rayOrigin + t * rayDirection;

        glm::vec3 lineDir = glm::normalize(segmentEnd - segmentStart);
        glm::vec3 t_temp(0.0f);

        if (rayIntersectsSphere(segmentStart, lineDir, samplePoint, 7.5f, t_temp)) {
            return true;
        }
    }
    return false;
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