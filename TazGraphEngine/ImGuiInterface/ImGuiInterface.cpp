#include "ImGuiInterface.h"

void ImGuiInterface::BeginRender() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void ImGuiInterface::RenderUI() {
    ImGui::Begin("Example Window");

    ImGui::Text("Hello, world!");
    if (ImGui::Button("Click me!")) {
        std::cout << "Button clicked!" << std::endl;
    }

    ImGui::End();
}

void ImGuiInterface::EndRender() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool ImGuiInterface::isMouseOnWidget(const std::string& widgetName)
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
