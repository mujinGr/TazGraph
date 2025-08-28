#include "EditorIMGUI.h"
#include "GECS/Core/GECSUtil.h"

EditorIMGUI::EditorIMGUI() {
}

EditorIMGUI::~EditorIMGUI() {

}


bool* EditorIMGUI::getDockspaceRef()
{
	return &_dockingEnabled;
}

bool EditorIMGUI::isMouseOnWidget(const std::string& widgetName)
{
	ImGuiContext& g = *ImGui::GetCurrentContext(); // Get ImGui context

	for (ImGuiWindow* window : g.Windows) {
		if (window->Name == widgetName && !(window->Hidden || (window->Active == false))) {
			ImVec2 widgetPos = window->Pos;
			ImVec2 widgetSize = window->Size;

			ImVec2 mousePos = ImGui::GetMousePos();
			
			return (mousePos.x >= widgetPos.x && mousePos.x <= (widgetPos.x + widgetSize.x) &&
				mousePos.y >= widgetPos.y && mousePos.y <= (widgetPos.y + widgetSize.y));
		}
	}

	return false;
}

