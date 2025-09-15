#include "HighlightBox.h"

void HighlightBox::OnImGuiRender()
{
	if (!ImGui::GetCurrentWindow())
		return;

	// Convert glm::vec2 to ImVec2
	ImVec2 p1(config.startPos.x, config.startPos.y);
	ImVec2 p2(config.currPos.x, config.currPos.y);


	std::cout << "start" << std::endl;
	std::cout << p1.x << p1.y << std::endl;
	std::cout << p2.x << p2.y << std::endl;


	// Ensure p1 is top-left and p2 is bottom-right
	ImVec2 topLeft(std::min(p1.x, p2.x), std::min(p1.y, p2.y));
	ImVec2 bottomRight(std::max(p1.x, p2.x), std::max(p1.y, p2.y));

	// Draw to the foreground draw list (on top of everything)
	ImDrawList* drawList = ImGui::GetForegroundDrawList();

	// Draw filled transparent rectangle
	drawList->AddRectFilled(topLeft, bottomRight, IM_COL32(0, 120, 215, 50));

	// Draw border
	drawList->AddRect(topLeft, bottomRight, IM_COL32(0, 120, 215, 255), 0.0f, 0, 2.0f);
}
