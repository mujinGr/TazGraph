#pragma once

#include "../../pch.h"

struct SelectionFrustum {
	glm::vec3 corners[8];
	glm::vec4 planes[6]; // left, right, bottom, top, near, far

	// Create frustum from screen-space selection box
	bool createFromSelectionBox(const glm::vec2& startPos, const glm::vec2& endPos,
		ICamera* camera, float nearZ = 100.0f, float farZ = 300000.0f) {
		// Ensure min/max order
		glm::vec2 minPos = glm::min(startPos, endPos);
		glm::vec2 maxPos = glm::max(startPos, endPos);

		// Convert screen coordinates to world coordinates at different Z depths
		glm::vec3 nearBottomLeft = camera->convertScreenToWorldDistance(glm::vec2(minPos.x, maxPos.y), nearZ);
		glm::vec3 nearBottomRight = camera->convertScreenToWorldDistance(glm::vec2(maxPos.x, maxPos.y), nearZ);
		glm::vec3 nearTopLeft = camera->convertScreenToWorldDistance(glm::vec2(minPos.x, minPos.y), nearZ);
		glm::vec3 nearTopRight = camera->convertScreenToWorldDistance(glm::vec2(maxPos.x, minPos.y), nearZ);

		glm::vec3 farBottomLeft = camera->convertScreenToWorldDistance(glm::vec2(minPos.x, maxPos.y), farZ);
		glm::vec3 farBottomRight = camera->convertScreenToWorldDistance(glm::vec2(maxPos.x, maxPos.y), farZ);
		glm::vec3 farTopLeft = camera->convertScreenToWorldDistance(glm::vec2(minPos.x, minPos.y), farZ);
		glm::vec3 farTopRight = camera->convertScreenToWorldDistance(glm::vec2(maxPos.x, minPos.y), farZ);

		// Store corners
		corners[0] = nearBottomLeft;
		corners[1] = nearBottomRight;
		corners[2] = nearTopRight;
		corners[3] = nearTopLeft;
		corners[4] = farBottomLeft;
		corners[5] = farBottomRight;
		corners[6] = farTopRight;
		corners[7] = farTopLeft;

		// Calculate frustum planes (normal pointing inward)
		// Left plane
		planes[0] = calculatePlane(nearTopLeft, nearBottomLeft, farBottomLeft);
		// Right plane
		planes[1] = calculatePlane(nearBottomRight, nearTopRight, farTopRight);
		// Bottom plane
		planes[2] = calculatePlane(nearBottomLeft, nearBottomRight, farBottomRight);
		// Top plane
		planes[3] = calculatePlane(nearTopRight, nearTopLeft, farTopLeft);
		// Near plane
		planes[4] = calculatePlane(nearTopLeft, nearTopRight, nearBottomRight);
		// Far plane
		planes[5] = calculatePlane(farTopRight, farTopLeft, farBottomLeft);

		bool isValid = true;
		for (int i = 0; i < 6; ++i) {
			if (glm::any(glm::isnan(glm::vec3(planes[i]))) || std::isnan(planes[i].w)) {
				isValid = false;
				break;
			}
		}

		return isValid;
	}

private:
	glm::vec4 calculatePlane(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) {
		glm::vec3 v1 = p2 - p1;
		glm::vec3 v2 = p3 - p1;
		glm::vec3 normal = glm::normalize(glm::cross(v1, v2));
		float d = -glm::dot(normal, p1);
		return glm::vec4(normal, d);
	}
};