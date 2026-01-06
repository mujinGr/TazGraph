#include "HelpPanel.h"

void HelpPanel::update(float deltaTime)
{
	// subComponents update
}

void HelpPanel::OnImGuiRender()
{

	if (showHelpPanel)
	{
		// Center the window when it first appears
		if (firstTime)
		{
			ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiCond_Appearing);
			firstTime = false;
		}

		// Window flags for better appearance
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoSavedSettings;

		ImGui::Begin("Help - TazGraph User Guide", &showHelpPanel, window_flags);

		// Make the window resizeable

		// Center the title text
		ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("TazGraph User Guide").x) * 0.5f);
		ImGui::TextColored(ImVec4(0.2f, 0.6f, 1.0f, 1.0f), "TazGraph User Guide");
		ImGui::Separator();

		// Add tabs for different sections
		if (ImGui::BeginTabBar("HelpTabs"))
		{
			// Camera Controls Tab
			if (ImGui::BeginTabItem("Camera Controls"))
			{
				RenderCameraControls();
				ImGui::EndTabItem();
			}

			// Shortcuts Tab
			if (ImGui::BeginTabItem("Keyboard Shortcuts"))
			{
				RenderKeyboardShortcuts();
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
	}
	else
	{
		firstTime = true; // Reset for next time window opens
	}
}

void HelpPanel::RenderCameraControls()
{
	ImGui::Spacing();
	ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.3f, 1.0f), "Camera Movement:");
	ImGui::Separator();

	// Movement controls in a table
	if (ImGui::BeginTable("CameraMovement", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit))
	{
		ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed, 100);
		ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthStretch);

		ImGui::TableHeadersRow();

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "W / Up Arrow");
		ImGui::TableNextColumn();
		ImGui::Text("Move camera forward");

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "S / Down Arrow");
		ImGui::TableNextColumn();
		ImGui::Text("Move camera backward");

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "A / Left Arrow");
		ImGui::TableNextColumn();
		ImGui::Text("Move camera left");

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "D / Right Arrow");
		ImGui::TableNextColumn();
		ImGui::Text("Move camera right");

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "E");
		ImGui::TableNextColumn();
		ImGui::Text("Move camera up (elevate)");

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "R");
		ImGui::TableNextColumn();
		ImGui::Text("Move camera down (descend)");

		ImGui::EndTable();
	}

	ImGui::Spacing();
	ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.3f, 1.0f), "Camera Rotation & Aim:");
	ImGui::Separator();

	// Mouse controls
	ImGui::BulletText("Middle Mouse Button: Click and drag to rotate camera");
	ImGui::BulletText("Middle Mouse Button + CTRL: Click and drag to pan camera");

	ImGui::Spacing();
	ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.3f, 1.0f), "Zoom Controls:");
	ImGui::Separator();

	ImGui::BulletText("Mouse Scroll: Zoom in/out");

}

void HelpPanel::RenderKeyboardShortcuts()
{
	ImGui::Spacing();
	ImGui::TextColored(ImVec4(0.9f, 0.5f, 0.2f, 1.0f), "Essential Shortcuts:");
	ImGui::Separator();

	if (ImGui::BeginTable("EssentialShortcuts", 3,
		ImGuiTableFlags_Borders |
		ImGuiTableFlags_RowBg |
		ImGuiTableFlags_SizingFixedFit))
	{
		ImGui::TableSetupColumn("Shortcut", ImGuiTableColumnFlags_WidthFixed, 120);
		ImGui::TableSetupColumn("Command", ImGuiTableColumnFlags_WidthFixed, 150);
		ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthStretch);

		ImGui::TableHeadersRow();

		// File Operations
		ImGui::TableNextRow();
		ImGui::TableNextColumn(); ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "CTRL + N");
		ImGui::TableNextColumn(); ImGui::Text("New Graph");
		ImGui::TableNextColumn(); ImGui::Text("Create new graph file");

		ImGui::TableNextRow();
		ImGui::TableNextColumn(); ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "CTRL + O");
		ImGui::TableNextColumn(); ImGui::Text("Open");
		ImGui::TableNextColumn(); ImGui::Text("Open existing graph file");

		ImGui::TableNextRow();
		ImGui::TableNextColumn(); ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "CTRL + S");
		ImGui::TableNextColumn(); ImGui::Text("Save");
		ImGui::TableNextColumn(); ImGui::Text("Save current graph");

		ImGui::TableNextRow();
		ImGui::TableNextColumn(); ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "CTRL + SHIFT + S");
		ImGui::TableNextColumn(); ImGui::Text("Save As");
		ImGui::TableNextColumn(); ImGui::Text("Save graph with new name");

		// Edit Operations
		ImGui::TableNextRow();
		ImGui::TableNextColumn(); ImGui::TextColored(ImVec4(0.2f, 0.6f, 0.8f, 1.0f), "CTRL + Z");
		ImGui::TableNextColumn(); ImGui::Text("Undo");
		ImGui::TableNextColumn(); ImGui::Text("Undo last action");

		ImGui::TableNextRow();
		ImGui::TableNextColumn(); ImGui::TextColored(ImVec4(0.2f, 0.6f, 0.8f, 1.0f), "CTRL + Y");
		ImGui::TableNextColumn(); ImGui::Text("Redo");
		ImGui::TableNextColumn(); ImGui::Text("Redo undone action");

		ImGui::TableNextRow();
		ImGui::TableNextColumn(); ImGui::TextColored(ImVec4(0.2f, 0.6f, 0.8f, 1.0f), "CTRL + C");
		ImGui::TableNextColumn(); ImGui::Text("Copy");
		ImGui::TableNextColumn(); ImGui::Text("Copy selection");

		ImGui::TableNextRow();
		ImGui::TableNextColumn(); ImGui::TextColored(ImVec4(0.2f, 0.6f, 0.8f, 1.0f), "CTRL + V");
		ImGui::TableNextColumn(); ImGui::Text("Paste");
		ImGui::TableNextColumn(); ImGui::Text("Paste clipboard");

		ImGui::TableNextRow();
		ImGui::TableNextColumn(); ImGui::TextColored(ImVec4(0.2f, 0.6f, 0.8f, 1.0f), "CTRL + X");
		ImGui::TableNextColumn(); ImGui::Text("Cut");
		ImGui::TableNextColumn(); ImGui::Text("Cut selection to clipboard");

		ImGui::TableNextRow();
		ImGui::TableNextColumn(); ImGui::TextColored(ImVec4(0.2f, 0.6f, 0.8f, 1.0f), "DEL");
		ImGui::TableNextColumn(); ImGui::Text("Delete");
		ImGui::TableNextColumn(); ImGui::Text("Delete selection");

		// View Operations
		ImGui::TableNextRow();
		ImGui::TableNextColumn(); ImGui::TextColored(ImVec4(0.6f, 0.2f, 0.8f, 1.0f), "F1");
		ImGui::TableNextColumn(); ImGui::Text("Help");
		ImGui::TableNextColumn(); ImGui::Text("Show this help panel");

		ImGui::TableNextRow();
		ImGui::TableNextColumn(); ImGui::TextColored(ImVec4(0.6f, 0.2f, 0.8f, 1.0f), "F5");
		ImGui::TableNextColumn(); ImGui::Text("Refresh");
		ImGui::TableNextColumn(); ImGui::Text("Refresh view");

		ImGui::TableNextRow();
		ImGui::TableNextColumn(); ImGui::TextColored(ImVec4(0.6f, 0.2f, 0.8f, 1.0f), "F11");
		ImGui::TableNextColumn(); ImGui::Text("Fullscreen");
		ImGui::TableNextColumn(); ImGui::Text("Toggle fullscreen mode");

		ImGui::TableNextRow();
		ImGui::TableNextColumn(); ImGui::TextColored(ImVec4(0.6f, 0.2f, 0.8f, 1.0f), "ESC");
		ImGui::TableNextColumn(); ImGui::Text("Escape");
		ImGui::TableNextColumn(); ImGui::Text("Cancel current operation / Close dialog");

		// Selection
		ImGui::TableNextRow();
		ImGui::TableNextColumn(); ImGui::TextColored(ImVec4(0.8f, 0.6f, 0.2f, 1.0f), "CTRL + A");
		ImGui::TableNextColumn(); ImGui::Text("Select All");
		ImGui::TableNextColumn(); ImGui::Text("Select all nodes and edges");

		ImGui::TableNextRow();
		ImGui::TableNextColumn(); ImGui::TextColored(ImVec4(0.8f, 0.6f, 0.2f, 1.0f), "CTRL + D");
		ImGui::TableNextColumn(); ImGui::Text("Deselect");
		ImGui::TableNextColumn(); ImGui::Text("Deselect all");

		ImGui::TableNextRow();
		ImGui::TableNextColumn(); ImGui::TextColored(ImVec4(0.8f, 0.6f, 0.2f, 1.0f), "TAB");
		ImGui::TableNextColumn(); ImGui::Text("Next");
		ImGui::TableNextColumn(); ImGui::Text("Cycle through selection");

		ImGui::EndTable();
	}

	ImGui::Spacing();
	ImGui::TextColored(ImVec4(0.9f, 0.5f, 0.2f, 1.0f), "Mouse Shortcuts:");
	ImGui::Separator();

	ImGui::BulletText("Middle Click + Drag: Rotate camera");
	ImGui::BulletText("Right Click + Drag: Pan camera");
	ImGui::BulletText("Scroll Wheel: Zoom in/out");
	ImGui::BulletText("CTRL + Scroll: Fine zoom");
	ImGui::BulletText("SHIFT + Click: Add to selection");
	ImGui::BulletText("CTRL + Click: Toggle selection");
	ImGui::BulletText("Double Click: Focus on object");
}