#pragma once

#include "../../pch.h"

#include "./SelectionFrustum.h"
constexpr float LINE_THRESHOLD = 5.0f;
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

inline bool checkCollision3D(const glm::vec3& centerA, const glm::vec3& halfSizeA,
	const glm::vec3& centerB, const glm::vec3& halfSizeB,
	float padding = 0.0f) {

	glm::vec3 paddedHalfSizeA = halfSizeA + glm::vec3(padding);
	glm::vec3 paddedHalfSizeB = halfSizeB + glm::vec3(padding);


	return !(centerA.x + paddedHalfSizeA.x <= centerB.x - paddedHalfSizeB.x ||
		centerA.x - paddedHalfSizeA.x >= centerB.x + paddedHalfSizeB.x ||
		centerA.y + paddedHalfSizeA.y <= centerB.y - paddedHalfSizeB.y ||
		centerA.y - paddedHalfSizeA.y >= centerB.y + paddedHalfSizeB.y ||
		centerA.z + paddedHalfSizeA.z <= centerB.z - paddedHalfSizeB.z ||
		centerA.z - paddedHalfSizeA.z >= centerB.z + paddedHalfSizeB.z);
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

	float A = glm::dot(rayDirection, rayDirection);
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

inline bool sphereIntersectsBox(
	const glm::vec3& sphereCenter, float sphereRadius,
	const glm::vec3& boxMin, const glm::vec3& boxMax,
	glm::vec3& intersectionPoint)
{
	glm::vec3 closestPoint = glm::clamp(sphereCenter, boxMin, boxMax);

	intersectionPoint = closestPoint;

	glm::vec3 diff = closestPoint - sphereCenter;
	float distanceSquared = glm::dot(diff, diff);

	return distanceSquared <= (sphereRadius * sphereRadius);
}

inline bool rayIntersectsBox(
	const glm::vec3& rayOrigin,
	const glm::vec3& rayDirection,
	const glm::vec3& boxMin,
	const glm::vec3& boxMax,
	glm::vec3& intersectionPoint,
	float* tOut = nullptr
) {
	float tMin = 0.0f;
	float tMax = std::numeric_limits<float>::max();

	// Test each axis slab
	for (int i = 0; i < 3; i++) {
		if (abs(rayDirection[i]) < 1e-8f) {
			// Ray is parallel to slab, check if origin is inside
			if (rayOrigin[i] < boxMin[i] || rayOrigin[i] > boxMax[i]) {
				return false;
			}
		}
		else {
			// Compute intersection t values for near and far plane
			float invD = 1.0f / rayDirection[i];
			float t1 = (boxMin[i] - rayOrigin[i]) * invD;
			float t2 = (boxMax[i] - rayOrigin[i]) * invD;

			// Ensure t1 is the near plane
			if (t1 > t2) std::swap(t1, t2);

			// Update tMin and tMax
			tMin = std::max(tMin, t1);
			tMax = std::min(tMax, t2);

			// Early exit if no overlap
			if (tMin > tMax) {
				return false;
			}
		}
	}

	// If tMax < 0, box is behind ray
	if (tMax < 0) {
		return false;
	}

	// tMin is the intersection distance
	float t = (tMin >= 0) ? tMin : tMax;
	intersectionPoint = rayOrigin + t * rayDirection;

	if (tOut) {
		*tOut = t;
	}

	return true;
}



inline bool rayIntersectsLineSegment(
	const glm::vec3& rayOrigin,
	const glm::vec3& rayDirection,
	const glm::vec3& segmentStart,
	const glm::vec3& segmentEnd
) {
	glm::vec3 lineVec = segmentEnd - segmentStart;
	glm::vec3 w = rayOrigin - segmentStart;

	float a = glm::dot(rayDirection, rayDirection);
	float b = glm::dot(rayDirection, lineVec);
	float c = glm::dot(lineVec, lineVec);
	float d = glm::dot(rayDirection, w);
	float e = glm::dot(lineVec, w);

	float denom = a * c - b * b;

	if (abs(denom) < 1e-6f) {
		return false; // Lines are parallel
	}

	float s = (b * e - c * d) / denom;
	float t = (a * e - b * d) / denom;

	// Clamp t to line segment bounds
	t = glm::clamp(t, 0.0f, 1.0f);

	// Check if ray parameter is positive
	if (s < 0) {
		return false;
	}

	glm::vec3 pointOnRay = rayOrigin + s * rayDirection;
	glm::vec3 pointOnSegment = segmentStart + t * lineVec;

	float distance = glm::distance(pointOnRay, pointOnSegment);

	if (distance <= LINE_THRESHOLD) {
		return true;
	}

	return false;

}

inline bool checkCircleLineCollision(glm::vec2 center, int circleRadius, glm::vec2 lineStartPoint, glm::vec2 lineEndPoint) {
	float dist = pointLineDistance(center, lineStartPoint, lineEndPoint);

	if (dist <= circleRadius) {
		float dx = lineEndPoint.x - lineStartPoint.x;
		float dy = lineEndPoint.y - lineStartPoint.y;
		float t = ((center.x - lineStartPoint.x) * dx + (center.y - lineStartPoint.y) * dy) / (dx * dx + dy * dy);

		t = std::max(0.0f, std::min(1.0f, t));

		float closestX = lineStartPoint.x + t * dx;
		float closestY = lineStartPoint.y + t * dy;

		float distanceToCircle = std::sqrt((closestX - center.x) * (closestX - center.x) +
			(closestY - center.y) * (closestY - center.y));
		return distanceToCircle <= circleRadius;
	}

	return false;
}

inline bool isPointInFrustum(const glm::vec3& point, const SelectionFrustum& frustum) {
	for (int i = 0; i < 6; i++) {
		float distance = glm::dot(glm::vec3(frustum.planes[i]), point) + frustum.planes[i].w;
		if (distance < 0) {
			return false; // Point is outside this plane
		}
	}
	return true;
}
