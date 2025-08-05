#pragma once

#include "GECS/Components.h"
#include "GECS/UtilComponents.h"
#include <ImGuizmo/ImGuizmo.h>
#include <BaseFPSLimiter/BaseFPSLimiter.h>

// Additional utility functions for common view orientations
namespace OrientationBoxPresets {
    inline glm::mat4 GetFrontView() {
        return glm::mat4(1.0f); // Identity - looking down -Z
    }

    inline glm::mat4 GetBackView() {
        return glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(0, 1, 0));
    }

    inline glm::mat4 GetLeftView() {
        return glm::rotate(glm::mat4(1.0f), -glm::half_pi<float>(), glm::vec3(0, 1, 0));
    }

    inline glm::mat4 GetRightView() {
        return glm::rotate(glm::mat4(1.0f), glm::half_pi<float>(), glm::vec3(0, 1, 0));
    }

    inline glm::mat4 GetTopView() {
        return glm::rotate(glm::mat4(1.0f), -glm::half_pi<float>(), glm::vec3(1, 0, 0));
    }

    inline glm::mat4 GetBottomView() {
        return glm::rotate(glm::mat4(1.0f), glm::half_pi<float>(), glm::vec3(1, 0, 0));
    }
}

class OrientationBox {
public:
    void Create(ImVec2 viewportPos, ImVec2 viewportSize);
    void UpdateCameraFromMatrix();


private:
    glm::mat4 _boxViewMatrix = glm::mat4(1.0f);

    // State
    bool _isHovered = false;
    bool _isUsing = false;
};