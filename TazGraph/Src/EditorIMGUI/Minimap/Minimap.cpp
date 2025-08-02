#include "./Minimap.h"

void Minimap::Create(ImVec2 viewportPos, ImVec2 viewportSize,
    const glm::mat4& view, const glm::mat4& proj,
    std::shared_ptr<PerspectiveCamera> camera) {

    const float minimapSize = 200.0f; // Size of the minimap
    const float minimapPadding = 10.0f;
    const float minimapWorldSize = 100.0f; // How much world space the minimap shows

    // Position minimap in top-right corner
    ImVec2 minimapPos = ImVec2(
        viewportPos.x + viewportSize.x - minimapSize - minimapPadding,
        viewportPos.y + minimapPadding
    );

    // Set ImGuizmo for minimap
    ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
    ImGuizmo::SetRect(minimapPos.x, minimapPos.y, minimapSize, minimapSize);

    // Draw minimap background
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(
        minimapPos,
        ImVec2(minimapPos.x + minimapSize, minimapPos.y + minimapSize),
        IM_COL32(40, 40, 40, 200)  // Semi-transparent dark background
    );

    // Draw minimap border
    drawList->AddRect(
        minimapPos,
        ImVec2(minimapPos.x + minimapSize, minimapPos.y + minimapSize),
        IM_COL32(100, 100, 100, 255),
        0.0f, 0, 2.0f
    );


    // Draw objects/entities on minimap
    DrawMinimapObjects(minimapPos, minimapSize, view, proj);

    // Draw camera frustum on minimap
    //DrawCameraFrustumOnMinimap(minimapPos, minimapSize, view, proj, minimapView, minimapProj);

    // Draw camera position indicator
   // DrawCameraIndicator(minimapPos, minimapSize, cameraPos, minimapView, minimapProj);

    // Add minimap label
    drawList->AddText(
        ImVec2(minimapPos.x + 5, minimapPos.y + 5),
        IM_COL32(255, 255, 255, 255),
        "Minimap"
    );

}

void Minimap::DrawMinimapObjects(ImVec2 minimapPos, float minimapSize,
    const glm::mat4& minimapView, const glm::mat4& minimapProj) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // Example: Draw some objects on the minimap
    // You would replace this with your actual scene objects
    std::vector<glm::vec3> objectPositions = {
        glm::vec3(10.0f, 0.0f, 10.0f),
        glm::vec3(-15.0f, 0.0f, 5.0f),
        glm::vec3(0.0f, 0.0f, -20.0f),
        glm::vec3(25.0f, 0.0f, -10.0f)
    };

    for (const auto& objPos : objectPositions) {
        // Transform world position to minimap screen coordinates
        glm::vec4 clipPos = minimapProj * minimapView * glm::vec4(objPos, 1.0f);
        if (clipPos.w > 0) {
            glm::vec2 ndcPos = glm::vec2(clipPos.x / clipPos.w, clipPos.y / clipPos.w);

            // Convert NDC to screen coordinates
            ImVec2 screenPos = ImVec2(
                minimapPos.x + (ndcPos.x * 0.5f + 0.5f) * minimapSize,
                minimapPos.y + (-ndcPos.y * 0.5f + 0.5f) * minimapSize
            );

            // Draw object as a small circle
            drawList->AddCircleFilled(screenPos, 3.0f, IM_COL32(0, 255, 0, 255), 8);
            drawList->AddCircle(screenPos, 3.0f, IM_COL32(0, 150, 0, 255), 8, 1.0f);
        }
    }
}
