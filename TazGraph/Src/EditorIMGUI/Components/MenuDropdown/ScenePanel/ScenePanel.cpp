#include "ScenePanel.h"

void ScenePanel::update(float deltaTime)
{
    // subComponents update
}

void ScenePanel::OnImGuiRender()
{
    ImGui::Begin("ScenePanel", &showScenePanel);
    // Background color control
    ImGui::Text("Background Color");

    if (ImGui::ColorEdit4("##BGColor", config.scene->backgroundColor)) {
    }
    ImGui::End();
}
