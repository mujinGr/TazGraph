#pragma once

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include "Camera2.5D/ICamera.h"


namespace TazGraphEngine {


	inline void drawTextAtWorldPositionPerspective(const glm::vec3& worldPos, const char* text,
		const glm::vec4& color, ICamera* camera) {
		glm::mat4 view = camera->getViewMatrix();
		glm::mat4 proj = camera->getProjMatrix();

		// Get actual viewport information
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();

		// Convert world to view space first
		glm::vec4 viewPos = view * glm::vec4(worldPos, 1.0f);

		// Check if point is behind camera (early exit)
		if (viewPos.z > 0.0f) {
			return; // Point is behind camera
		}

		// Convert to clip space
		glm::vec4 clipPos = proj * viewPos;

		// Perspective division (only if w > 0)
		if (clipPos.w <= 0.0f) {
			return; // Point is at or behind near plane
		}

		// Normalize device coordinates (NDC)
		glm::vec3 ndc;
		ndc.x = clipPos.x / clipPos.w;
		ndc.y = clipPos.y / clipPos.w;
		ndc.z = clipPos.z / clipPos.w;

		// Check if point is within NDC bounds
		if (ndc.x < -1.0f || ndc.x > 1.0f || ndc.y < -1.0f || ndc.y > 1.0f) {
			return; // Point is outside view frustum
		}

		// Convert NDC to viewport coordinates
		glm::vec2 screenPos;
		screenPos.x = (ndc.x * 0.5f + 0.5f) * viewportSize.x;
		screenPos.y = (1.0f - (ndc.y * 0.5f + 0.5f)) * viewportSize.y; // Flip Y for screen coordinates

		// Add viewport offset to get actual screen position
		ImVec2 finalScreenPos = ImVec2(
			cursorPos.x + screenPos.x,
			cursorPos.y + screenPos.y
		);

		// Calculate distance-based scaling
		float distance = -viewPos.z; // Distance in view space (positive value)
		float baseFontSize = 16.0f;
		float scaleMultiplier = 1000.0f; // Adjust this to control scaling sensitivity
		float scale = scaleMultiplier / std::max(distance, 1.0f); // Prevent division by zero
		float fontSize = glm::clamp(baseFontSize * scale, 4.0f, 48.0f); // Wider clamp range

		// Get draw list for the current window (not foreground)
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		// Calculate text size with the scaled font
		ImFont* font = ImGui::GetIO().Fonts->Fonts[0];
		ImVec2 textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, text);

		// Center the text at the calculated position
		ImVec2 textPos = ImVec2(
			finalScreenPos.x - textSize.x * 0.5f,
			finalScreenPos.y - textSize.y * 0.5f
		);

		// Add text to draw list
		drawList->AddText(
			font,
			fontSize,
			textPos,
			ImColor(color.r, color.g, color.b, color.a),
			text
		);
	}
}
