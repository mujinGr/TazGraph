#pragma once

#include "GECS/Components.h"
#include "GECS/UtilComponents.h"
#include <ImGuizmo/ImGuizmo.h>
#include <BaseFPSLimiter/BaseFPSLimiter.h>

class OrientationBox {
public:
    void Create(ImVec2 viewportPos, ImVec2 viewportSize);
    void UpdateCameraFromMatrix();


private:
    glm::mat4 _boxViewMatrix = glm::mat4(1.0f);

};