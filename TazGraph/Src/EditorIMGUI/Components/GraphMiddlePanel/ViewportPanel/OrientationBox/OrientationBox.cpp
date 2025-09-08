#include "./OrientationBox.h"

void OrientationBox::OnImGuiRender() {

    const float orientationBoxSize = 200.0f; // Size of the minimap
    const float orientationBoxPadding = 10.0f;

    // Position minimap in top-right corner
    ImVec2 orientationBoxPos = ImVec2(
        config.viewportPos.x + config.viewportSize.x - orientationBoxSize - orientationBoxPadding,
        config.viewportPos.y + config.viewportSize.y - orientationBoxSize - orientationBoxPadding
    );

    // Set ImGuizmo for minimap
    ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
    ImGuizmo::SetRect(orientationBoxPos.x, orientationBoxPos.y, orientationBoxSize, orientationBoxSize);

    // Draw minimap background
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(
        orientationBoxPos,
        ImVec2(orientationBoxPos.x + orientationBoxSize, orientationBoxPos.y + orientationBoxSize),
        IM_COL32(40, 40, 40, 200)  // Semi-transparent dark background
    );

    // Draw minimap border
    drawList->AddRect(
        orientationBoxPos,
        ImVec2(orientationBoxPos.x + orientationBoxSize, orientationBoxPos.y + orientationBoxSize),
        IM_COL32(100, 100, 100, 255),
        0.0f, 0, 2.0f
    );

    // Add minimap label
    drawList->AddText(
        ImVec2(orientationBoxPos.x + 5, orientationBoxPos.y + 5),
        IM_COL32(255, 255, 255, 255),
        "Orientation Box"
    );


    std::shared_ptr<PerspectiveCamera> main_camera2D =
        std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));


    if (!main_camera2D) return;

    _boxViewMatrix = main_camera2D->getViewMatrix();
    
    float* matrixPtr = &_boxViewMatrix[0][0];
    ImGuizmo::ViewManipulate(
        matrixPtr,                                    // View matrix to manipulate
        8.0f,                                        // Camera distance (affects cube appearance)
        orientationBoxPos,                           // Position
        ImVec2(orientationBoxSize, orientationBoxSize), // Size
        0x10101010                                   // Background color (dark gray)
    );

    if (_boxViewMatrix != main_camera2D->getViewMatrix()) {
        UpdateCameraFromMatrix();
    }
}


void OrientationBox::UpdateCameraFromMatrix() {
    std::shared_ptr<PerspectiveCamera> main_camera2D =
        std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));


    glm::mat3 rotation = glm::mat3(_boxViewMatrix);

    // Calculate new camera vectors
    glm::vec3 forward = glm::transpose(rotation) * glm::vec3(0, 0, -1); 
    glm::vec3 up = main_camera2D->getUpDir();       

    // Get current camera state
    glm::vec3 currentPos = main_camera2D->getPosition();
    glm::vec3 currentTarget = glm::vec3(0); 

    // Calculate new position maintaining the same distance from target
    float distance = glm::length(currentPos - currentTarget);
    glm::vec3 newPosition = currentTarget - forward * distance;

    // Update camera
    main_camera2D->setPosition(newPosition);
    main_camera2D->setAimPos(glm::vec3(0.0f, 0.0f, 0.0f));
}

void OrientationBox::setConfig(const OrientationBoxConfig& cfg)
{
    config = cfg;
}
