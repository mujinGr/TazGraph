#include "ImGuiInterface.h"


ImGuiInterface::ImGuiInterface() {
}

ImGuiInterface::~ImGuiInterface() {
}

//void ImGuiInterface::SetupImGui(SDL_Window sdlWindow, SDL_GLContext glContext) {
//    ImGui::CreateContext();
//    ImPlot::CreateContext();
//    // Setup Platform/Renderer bindings
//    ImGui_ImplSDL2_InitForOpenGL(sdlWindow, &glContext);
//    ImGui_ImplOpenGL3_Init("#version 330"); // Or whatever GLSL version suits your needs
//}

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