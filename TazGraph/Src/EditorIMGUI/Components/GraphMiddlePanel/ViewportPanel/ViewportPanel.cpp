#include "ViewportPanel.h"

void ViewportPanel::OnImGuiRender()
{

	updateIsMouseInSecondColumn();

	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

	// Render your scene texture
	ImGui::Image(
		reinterpret_cast<void*>(static_cast<uintptr_t>(config.c_fb->_framebufferTexture)),
		viewportPanelSize,
		ImVec2(0, 1),
		ImVec2(1, 0)
	);

	*config.c_storedWindowPos = ImGui::GetWindowPos();
	*config.c_storedWindowSize = viewportPanelSize;

	// Check if we have valid size and camera
	if (viewportPanelSize.x <= 0 || viewportPanelSize.y <= 0) {
		return;
	}

	// Get camera matrices
	std::shared_ptr<PerspectiveCamera> main_camera2D =
		std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	std::shared_ptr<OrthoCamera> hud_camera2D =
		std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	std::shared_ptr<OrthoCamera> minimap_camera2D =
		std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("minimap"));


	if (!minimap_camera2D) {
		return;
	}

	// Set up ImGuizmo to render over the image
	ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
	ImGuizmo::SetRect(pos.x, pos.y, viewportPanelSize.x, viewportPanelSize.y);

	// Only enable gizmo when mouse is over the viewport
	bool isHovered = ImGui::IsItemHovered();
	ImGuizmo::Enable(isHovered);

	getSubcomponent<Minimap>()->setConfig({
	.textureID = config.c_minimap_fb->_framebufferTexture,
	.viewportPos = pos,
	.viewportSize = viewportPanelSize
		});
	getSubcomponent<Minimap>()->OnImGuiRender();

	getSubcomponent<OrientationBox>()->setConfig({
		.viewportPos = pos,
		.viewportSize = viewportPanelSize
		});
	getSubcomponent<OrientationBox>()->OnImGuiRender();

	getSubcomponent<HighlightBox>()->setConfig({
		.startPos = config.startPos,
		.currPos = config.currPos
		});
	getSubcomponent<HighlightBox>()->OnImGuiRender();

}


void ViewportPanel::updateIsMouseInSecondColumn() {
	// cant check with the checkIfMouseIsInWidget because it is a child of a window
	ImVec2 columnStartPos = ImGui::GetCursorScreenPos();
	ImVec2 columnSize = ImVec2(ImGui::GetColumnWidth(), ImGui::GetContentRegionAvail().y);

	ImVec2 mousePos = ImGui::GetMousePos();
	isMouseInSecondColumn = (mousePos.x >= columnStartPos.x && mousePos.x <= (columnStartPos.x + columnSize.x) &&
		mousePos.y >= columnStartPos.y && mousePos.y <= (columnStartPos.y + columnSize.y));
}