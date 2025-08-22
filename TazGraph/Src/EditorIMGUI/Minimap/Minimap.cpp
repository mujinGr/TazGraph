#include "./Minimap.h"

void Minimap::Create(uint32_t m_textureID, const BaseFPSLimiter& baseFPSLimiter, Manager& manager, ImVec2 viewportPos, ImVec2 viewportSize) {

    const float minimapSize = 200.0f; // Size of the minimap
    const float minimapPadding = 10.0f;

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
    drawList->AddImage(
        reinterpret_cast<void*>(static_cast<uintptr_t>(m_textureID)),
        minimapPos,
        ImVec2(minimapPos.x + minimapSize, minimapPos.y + minimapSize),
        ImVec2(0, 1),   // UV top-left
        ImVec2(1, 0)    // UV bottom-right (flip Y)
    );

    // Draw minimap border
    drawList->AddRect(
        minimapPos,
        ImVec2(minimapPos.x + minimapSize, minimapPos.y + minimapSize),
        IM_COL32(100, 100, 100, 255),
        0.0f, 0, 2.0f
    );

    DrawCameraFrustumOnMinimap(minimapPos, minimapSize);
    DrawCameraIndicator(minimapPos, minimapSize);

    // Add minimap label
    drawList->AddText(
        ImVec2(minimapPos.x + 5, minimapPos.y + 5),
        IM_COL32(255, 255, 255, 255),
        "Minimap"
    );

}

void Minimap::DrawCameraFrustumOnMinimap(ImVec2 minimapPos, float minimapSize) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    std::shared_ptr<PerspectiveCamera> main_camera2D =
        std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
       
    std::shared_ptr<OrthoCamera> minimap_camera2D =
        std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("minimap"));

    glm::mat4 tempProj = glm::perspective(glm::radians(main_camera2D->fov), main_camera2D->aspect, 5.0f, 5000.0f); // instead of 1,000,000
    glm::mat4 invViewProj = glm::inverse(tempProj * main_camera2D->getViewMatrix());
    // Get inverse matrices to reconstruct frustum
    //glm::mat4 invViewProj = glm::inverse(main_camera2D->getProjMatrix() * main_camera2D->getViewMatrix());

    // Define frustum corners in NDC space
    std::vector<glm::vec4> frustumCorners = {
        // Near plane
    glm::vec4(-1, -1, -1, 1), glm::vec4(1, -1, -1, 1),
    glm::vec4(1, 1, -1, 1),   glm::vec4(-1, 1, -1, 1),
    // Far plane
    glm::vec4(-1, -1,  1, 1), glm::vec4(1, -1,  1, 1),
    glm::vec4(1,  1,  1, 1),  glm::vec4(-1, 1,  1, 1)
    };

    // Transform frustum corners to world space, then to minimap space
    std::vector<ImVec2> minimapCorners;
    for (auto& corner : frustumCorners) {
        // Transform to world space
        glm::vec4 worldPos = invViewProj * corner;

        worldPos /= worldPos.w;

        // Transform to minimap clip space
        glm::vec4 minimapClip = minimap_camera2D->getProjMatrix() * minimap_camera2D->getViewMatrix() * worldPos;
        if (minimapClip.w > 0) {
            glm::vec2 ndcPos = glm::vec2(minimapClip.x / minimapClip.w, minimapClip.y / minimapClip.w);

            // Convert to screen coordinates
            ImVec2 screenPos = ImVec2(
                minimapPos.x + (ndcPos.x * 0.5f + 0.5f) * minimapSize,
                minimapPos.y + (-ndcPos.y * 0.5f + 0.5f) * minimapSize
            );
            minimapCorners.push_back(screenPos);
        }
    }

    // Draw frustum outline
    if (minimapCorners.size() >= 8) {
        const ImU32 frustumColor = IM_COL32(255, 255, 0, 150);

        // Draw near plane
        for (int i = 0; i < 4; i++) {
            drawList->AddLine(minimapCorners[i], minimapCorners[(i + 1) % 4], frustumColor, 1.5f);
        }

        // Draw far plane
        for (int i = 4; i < 8; i++) {
            drawList->AddLine(minimapCorners[i], minimapCorners[4 + ((i - 4 + 1) % 4)], frustumColor, 1.5f);
        }

        // Draw connecting lines
        for (int i = 0; i < 4; i++) {
            drawList->AddLine(minimapCorners[i], minimapCorners[i + 4], frustumColor, 1.5f);
        }
    }

}


void Minimap::DrawCameraIndicator(ImVec2 minimapPos, float minimapSize) 
{
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    std::shared_ptr<PerspectiveCamera> main_camera2D =
        std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

    std::shared_ptr<OrthoCamera> minimap_camera2D =
        std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("minimap"));


    // Transform camera position to minimap coordinates
    glm::vec4 clipPos = minimap_camera2D->getProjMatrix() * minimap_camera2D->getViewMatrix() * glm::vec4(main_camera2D->eyePos , 1.0f);
    if (clipPos.w > 0) {
        glm::vec2 ndcPos = glm::vec2(clipPos.x / clipPos.w, clipPos.y / clipPos.w);

        ImVec2 screenPos = ImVec2(
            minimapPos.x + (ndcPos.x * 0.5f + 0.5f) * minimapSize,
            minimapPos.y + (-ndcPos.y * 0.5f + 0.5f) * minimapSize
        );

        // Draw camera as a triangle pointing in direction
        const float triangleSize = 5.0f;
        drawList->AddTriangleFilled(
            ImVec2(screenPos.x, screenPos.y - triangleSize),
            ImVec2(screenPos.x - triangleSize, screenPos.y + triangleSize),
            ImVec2(screenPos.x + triangleSize, screenPos.y + triangleSize),
            IM_COL32(255, 100, 100, 255)
        );

        // Draw outline
        drawList->AddTriangle(
            ImVec2(screenPos.x, screenPos.y - triangleSize),
            ImVec2(screenPos.x - triangleSize, screenPos.y + triangleSize),
            ImVec2(screenPos.x + triangleSize, screenPos.y + triangleSize),
            IM_COL32(255, 255, 255, 255), 1.5f
        );
    }
}