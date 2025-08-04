#pragma once

#include "GECS/Components.h"
#include "GECS/UtilComponents.h"
#include <ImGuizmo/ImGuizmo.h>
#include <BaseFPSLimiter/BaseFPSLimiter.h>


class Minimap {
public:
    void Create(const BaseFPSLimiter& baseFPSLimiter, Manager& manager, ImVec2 viewportPos, ImVec2 viewportSize);
    void DrawMinimapObjects(Manager& manager, ImVec2 minimapPos, float minimapSize);
    void DrawCameraFrustumOnMinimap(ImVec2 minimapPos, float minimapSize);
    void DrawCameraIndicator(ImVec2 minimapPos, float minimapSize);

private:
    float elapsed = 0.0f;
};