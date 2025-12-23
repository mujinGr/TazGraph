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
			ImGui::SetNextWindowSize(ImVec2(500, 600), ImGuiCond_Appearing);
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

			// Navigation Tab
			if (ImGui::BeginTabItem("Navigation"))
			{
				RenderNavigationControls();
				ImGui::EndTabItem();
			}

			// View Controls Tab
			if (ImGui::BeginTabItem("View Controls"))
			{
				RenderViewControls();
				ImGui::EndTabItem();
			}

			// Graph Operations Tab
			if (ImGui::BeginTabItem("Graph Operations"))
			{
				RenderGraphControls();
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
	ImGui::BulletText("Middle Mouse Button + ALT: Click and drag to pan camera");
	ImGui::BulletText("Middle Mouse Button + CTRL: Click and drag to zoom");

	ImGui::Spacing();
	ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.3f, 1.0f), "Zoom Controls:");
	ImGui::Separator();

	ImGui::BulletText("Mouse Scroll: Zoom in/out");
	ImGui::BulletText("+ / - Keys: Zoom in/out");
	ImGui::BulletText("Mouse Wheel Click + Drag: Adjust zoom level");

	ImGui::Spacing();
	ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.3f, 1.0f), "Camera Presets:");
	ImGui::Separator();

	if (ImGui::BeginTable("CameraPresets", 2, ImGuiTableFlags_BordersInnerV))
	{
		ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed, 100);
		ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthStretch);

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "F");
		ImGui::TableNextColumn();
		ImGui::Text("Focus camera on selected object");

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "Home");
		ImGui::TableNextColumn();
		ImGui::Text("Reset camera to default position");

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "Num 1-6");
		ImGui::TableNextColumn();
		ImGui::Text("Quick camera views (Front, Back, Top, etc.)");

		ImGui::EndTable();
	}
}

void HelpPanel::RenderNavigationControls()
{
	ImGui::Spacing();
	ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.6f, 1.0f), "Scene Navigation:");
	ImGui::Separator();

	ImGui::BulletText("Click and drag in empty space: Pan the view");
	ImGui::BulletText("Double-click on object: Focus camera on object");
	ImGui::BulletText("CTRL + Click: Add to selection");
	ImGui::BulletText("SHIFT + Click: Range selection");

	ImGui::Spacing();
	ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.6f, 1.0f), "Viewport Navigation:");
	ImGui::Separator();

	if (ImGui::BeginTable("ViewportNav", 2, ImGuiTableFlags_BordersInnerV))
	{
		ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed, 100);
		ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthStretch);

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "Spacebar");
		ImGui::TableNextColumn();
		ImGui::Text("Toggle between orbit and fly camera mode");

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "Tab");
		ImGui::TableNextColumn();
		ImGui::Text("Toggle between perspective and orthographic view");

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "G");
		ImGui::TableNextColumn();
		ImGui::Text("Toggle grid visibility");

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "B");
		ImGui::TableNextColumn();
		ImGui::Text("Toggle bounding boxes");

		ImGui::EndTable();
	}
}

void HelpPanel::RenderViewControls()
{
	ImGui::Spacing();
	ImGui::TextColored(ImVec4(0.8f, 0.4f, 0.8f, 1.0f), "Display Settings:");
	ImGui::Separator();

	if (ImGui::BeginTable("DisplaySettings", 2, ImGuiTableFlags_BordersInnerV))
	{
		ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed, 100);
		ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthStretch);

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "V");
		ImGui::TableNextColumn();
		ImGui::Text("Toggle between wireframe and solid view");

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "L");
		ImGui::TableNextColumn();
		ImGui::Text("Toggle lighting");

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "T");
		ImGui::TableNextColumn();
		ImGui::Text("Toggle textures");

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "C");
		ImGui::TableNextColumn();
		ImGui::Text("Toggle coordinate axes");

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "H");
		ImGui::TableNextColumn();
		ImGui::Text("Toggle this help panel");

		ImGui::EndTable();
	}

	ImGui::Spacing();
	ImGui::TextColored(ImVec4(0.8f, 0.4f, 0.8f, 1.0f), "Visual Aids:");
	ImGui::Separator();

	ImGui::BulletText("Press 'G' to show/hide the grid");
	ImGui::BulletText("Press 'X', 'Y', 'Z' to align camera to axis");
	ImGui::BulletText("Use number keys 1-4 for different visual styles");
	ImGui::BulletText("CTRL + S saves current view as preset");
}

void HelpPanel::RenderGraphControls()
{
	ImGui::Spacing();
	ImGui::TextColored(ImVec4(0.4f, 0.8f, 0.8f, 1.0f), "Graph Manipulation:");
	ImGui::Separator();

	if (ImGui::BeginTable("GraphControls", 2, ImGuiTableFlags_BordersInnerV))
	{
		ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed, 100);
		ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthStretch);

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "Left Click");
		ImGui::TableNextColumn();
		ImGui::Text("Select node or edge");

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "Drag");
		ImGui::TableNextColumn();
		ImGui::Text("Move selected nodes");

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "DEL / Backspace");
		ImGui::TableNextColumn();
		ImGui::Text("Delete selected nodes/edges");

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "CTRL + C");
		ImGui::TableNextColumn();
		ImGui::Text("Copy selection");

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "CTRL + V");
		ImGui::TableNextColumn();
		ImGui::Text("Paste selection");

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "CTRL + Z");
		ImGui::TableNextColumn();
		ImGui::Text("Undo");

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "CTRL + Y");
		ImGui::TableNextColumn();
		ImGui::Text("Redo");

		ImGui::EndTable();
	}

	ImGui::Spacing();
	ImGui::TextColored(ImVec4(0.4f, 0.8f, 0.8f, 1.0f), "Graph Creation:");
	ImGui::Separator();

	ImGui::BulletText("Double-click in empty space: Create new node");
	ImGui::BulletText("Drag from node to empty space: Create node with connection");
	ImGui::BulletText("Drag from node to node: Create edge");
	ImGui::BulletText("Right-click on node: Show context menu");
	ImGui::BulletText("Right-click in empty space: Show graph creation menu");
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