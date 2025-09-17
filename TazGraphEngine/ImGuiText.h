#pragma once

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include "Camera2.5D/ICamera.h"


namespace TazGraphEngine {


	inline void drawTextAtWorldPositionPerspective(const glm::vec3& worldPos, const char* text,
		const glm::vec4& color, ICamera* camera) {

		glm::mat4 view = camera->getViewMatrix();
		glm::mat4 proj = camera->getProjMatrix();
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();

		// Convert world to clip space
		glm::vec4 clipPos = proj * view * glm::vec4(worldPos, 1.0f);

		// Perspective division
		if (clipPos.w > 0.0f) {
			clipPos.x /= clipPos.w;
			clipPos.y /= clipPos.w;

			// Convert to screen coordinates
			glm::vec2 screenPos;
			screenPos.x = (clipPos.x * 0.5f + 0.5f) * viewportSize.x;
			screenPos.y = (-clipPos.y * 0.5f + 0.5f) * viewportSize.y;
			glm::vec4 viewPos = view * glm::vec4(worldPos, 1.0f);
			// Use clipPos.w (distance in clip space) for scaling
			float distance = -viewPos.z; // view space forward is -Z
			float baseFontSize = 16.0f;
			float scale = 1000.0f / distance;   // Adjust multiplier as needed
			float fontSize = glm::clamp(baseFontSize * scale, 6.0f, baseFontSize);

			ImDrawList* drawList = ImGui::GetForegroundDrawList();

			// Push scaled font
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
			ImGui::SetWindowFontScale(fontSize / baseFontSize);

			ImFont* font = ImGui::GetIO().Fonts->Fonts[0];
			ImVec2 textSize = ImGui::CalcTextSize(text);
			drawList->AddText(
				font, fontSize,
				ImVec2(screenPos.x - textSize.x * 0.5f, screenPos.y - textSize.y * 0.5f),
				ImColor(color.r, color.g, color.b, color.a),
				text);

			ImGui::SetWindowFontScale(1.0f);
			ImGui::PopFont();
		}
	}

}