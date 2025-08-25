#pragma once

#include "GECS/Components.h"
#include "GECS/UtilComponents.h"
#include <ImGuizmo/ImGuizmo.h>
#include <BaseFPSLimiter/BaseFPSLimiter.h>


struct MinimapConfig {
    uint32_t textureID;
    ImVec2 viewportPos;
    ImVec2 viewportSize;
};

class Minimap {
public:
    void OnImGuiRender();
    void DrawCameraFrustumOnMinimap(ImVec2 minimapPos, float minimapSize);
    void DrawCameraIndicator(ImVec2 minimapPos, float minimapSize);

    void setConfig(const MinimapConfig& cfg);
private:
    MinimapConfig config;
    float elapsed = 0.0f;
};