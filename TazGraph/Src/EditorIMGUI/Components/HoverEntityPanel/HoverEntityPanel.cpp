#include "HoverEntityPanel.h"

#ifdef defined(_WIN32) || defined(_WIN64)
#define safe_sprintf sprintf_s
#else
#define safe_sprintf sprintf
#endif

void HoverEntityPanel::OnImGuiRender()
{
	if (!config.hoveredEntity) return;

	const float hoveredEntityWindowSize = 220.0f;
	const float windowHeight = 120.0f; // Adjust height based on content

	// Position window near mouse cursor
	ImVec2 hoveredEntityWindowPos = ImVec2(config.mousePos.x + 10, config.mousePos.y - windowHeight);

	// Set up ImGuizmo for drawing

	// Get draw list for custom drawing
	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	ImGuizmo::SetDrawlist(drawList);
	// Draw background rectangle
	drawList->AddRectFilled(
		hoveredEntityWindowPos,
		ImVec2(hoveredEntityWindowPos.x + hoveredEntityWindowSize, hoveredEntityWindowPos.y + windowHeight),
		IM_COL32(40, 40, 40, 240) // Dark semi-transparent background
	);

	// Draw border
	drawList->AddRect(
		hoveredEntityWindowPos,
		ImVec2(hoveredEntityWindowPos.x + hoveredEntityWindowSize, hoveredEntityWindowPos.y + windowHeight),
		IM_COL32(100, 100, 100, 255),
		3.0f, // Corner rounding
		0,
		2.0f  // Border thickness
	);

	// Draw title
	drawList->AddText(
		ImVec2(hoveredEntityWindowPos.x + 10, hoveredEntityWindowPos.y + 10),
		IM_COL32(255, 255, 255, 255),
		"Hovered Entity"
	);

	// Current text position
	float currentY = hoveredEntityWindowPos.y + 35;
	const float lineHeight = 15.0f;
	const float textX = hoveredEntityWindowPos.x + 10;

	// Display entity information
	Node* node = dynamic_cast<Node*>(config.hoveredEntity);
	if (node) {
		// Entity ID
		char idText[64];
		safe_sprintf(idText, "Id: %d", config.hoveredEntity->getId());
		drawList->AddText(ImVec2(textX, currentY), IM_COL32(200, 200, 200, 255), idText);
		currentY += lineHeight;

		// Position
		TransformComponent* tr = &config.hoveredEntity->GetComponent<TransformComponent>();
		char posText[128];
		safe_sprintf(posText, "Position: (%.2f, %.2f)", tr->getPosition().x, tr->getPosition().y);
		drawList->AddText(ImVec2(textX, currentY), IM_COL32(200, 200, 200, 255), posText);
		currentY += lineHeight;

		// Size
		char sizeText[128];
		safe_sprintf(sizeText, "Size: (%.2f, %.2f)", tr->size.x, tr->size.y);
		drawList->AddText(ImVec2(textX, currentY), IM_COL32(200, 200, 200, 255), sizeText);
		currentY += lineHeight;

		// Grid position
		glm::vec3 cellBox = config.manager->grid->getCell(*config.hoveredEntity, config.manager->grid->getGridLevel())->boundingBox_origin;
		char gridText[128];
		safe_sprintf(gridText, "Grid: (%.2f, %.2f)", cellBox.x, cellBox.y);
		drawList->AddText(ImVec2(textX, currentY), IM_COL32(200, 200, 200, 255), gridText);
	}

	Link* link = dynamic_cast<Link*>(config.hoveredEntity);
	if (link) {
		char idText[64];
		safe_sprintf(idText, "Link Id: %d", config.hoveredEntity->getId());
		drawList->AddText(ImVec2(textX, currentY), IM_COL32(200, 200, 200, 255), idText);
	}

	Empty* empty = dynamic_cast<Empty*>(config.hoveredEntity);
	if (empty) {
		// Entity ID
		char idText[64];
		safe_sprintf(idText, "Empty Id: %d", config.hoveredEntity->getId());
		drawList->AddText(ImVec2(textX, currentY), IM_COL32(200, 200, 200, 255), idText);
		currentY += lineHeight;

		// Position
		TransformComponent* tr = &config.hoveredEntity->GetComponent<TransformComponent>();
		char posText[128];
		safe_sprintf(posText, "Position: (%.2f, %.2f)", tr->getPosition().x, tr->getPosition().y);
		drawList->AddText(ImVec2(textX, currentY), IM_COL32(200, 200, 200, 255), posText);
		currentY += lineHeight;

		// Size
		char sizeText[128];
		safe_sprintf(sizeText, "Size: (%.2f, %.2f)", tr->size.x, tr->size.y);
		drawList->AddText(ImVec2(textX, currentY), IM_COL32(200, 200, 200, 255), sizeText);
		currentY += lineHeight;

		// Grid position
		glm::vec3 cellBox = config.manager->grid->getCell(*config.hoveredEntity, config.manager->grid->getGridLevel())->boundingBox_origin;
		char gridText[128];
		safe_sprintf(gridText, "Grid: (%.2f, %.2f)", cellBox.x, cellBox.y);
		drawList->AddText(ImVec2(textX, currentY), IM_COL32(200, 200, 200, 255), gridText);
	}

}
