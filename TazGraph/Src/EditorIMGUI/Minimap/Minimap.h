#pragma once

#include "GECS/Components.h"
#include "GECS/UtilComponents.h"
#include <ImGuizmo/ImGuizmo.h>

class Minimap {
public:
    void Create(ImVec2 viewportPos, ImVec2 viewportSize,
        const glm::mat4& view, const glm::mat4& proj,
        std::shared_ptr<PerspectiveCamera> camera);
    void DrawMinimapObjects(ImVec2 minimapPos, float minimapSize,
        const glm::mat4& minimapView, const glm::mat4& minimapProj);
};