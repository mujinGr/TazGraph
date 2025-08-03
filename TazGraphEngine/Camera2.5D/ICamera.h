#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <SDL2/SDL.h>

enum class ViewMode {
    Y_UP,
    Z_UP
};

class ICamera {
public:
	int _screenWidth = 800, _screenHeight = 640;

    glm::vec3 eyePos{ 0,0,0 };
    glm::vec3 aimPos{ 0,0,0 };
    glm::vec3 upDir{ 0,-1,0 };
    float zFar = 1000000.0f;

    ViewMode currentViewMode = ViewMode::Y_UP;

    virtual ~ICamera() = default;

    // Initializes the camera2D.worldLocation with the screen's width and height
    virtual void init() = 0;

    // Updates the camera2D.worldLocation's matrix if there have been any changes
    virtual void update() = 0;

    // Converts screen coordinates to world coordinates
    virtual glm::vec2 convertScreenToWorld(glm::vec2 screenCoords) const = 0;

    // Returns the dimensions of the camera2D.worldLocation's view
    virtual glm::ivec2 getCameraDimensions() const = 0;

    // Returns the SDL_Rect representing the camera2D.worldLocation's viewport
    virtual SDL_FRect getCameraRect() const = 0;

    // Additional methods to expose camera2D.worldLocation properties as needed
    virtual glm::vec3 getPosition() const = 0;
    virtual void setPosition(const glm::vec3 newPosition) = 0;
    virtual void setPosition_X(const float newPosition) = 0;
    virtual void setPosition_Y(const float newPosition) = 0;
    virtual void setPosition_Z(const float newPosition) = 0;
    virtual float getScale() const = 0;
    virtual glm::mat4 getCameraMatrix() const = 0;
    virtual void setScale(float scale) = 0;

    virtual void makeCameraDirty() = 0;
    virtual bool hasChanged() = 0;
    virtual void refreshCamera() = 0;

    void updateCameraOrientation();

    void setOrientation(glm::vec3 eye, glm::vec3 target, glm::vec3 up);

	bool isPointInCameraView(const glm::vec4 point, float margin);

	// Function to cast a ray from screen coordinates into world space
    glm::vec3 castRayAt(const glm::vec2& screenPos);

    glm::vec3 getPointOnRayAtZ(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float desiredZ);

    void setViewMatrix(glm::mat4 newViewMatrix);

    glm::mat4 getViewMatrix();

    void setProjMatrix(glm::mat4 newProjMatrix);

    glm::mat4 getProjMatrix();

protected:
    glm::mat4 _projectionMatrix = glm::mat4(1.0f); // changed once in init
    glm::mat4 _viewMatrix = glm::mat4(1.0f);
    glm::mat4 _cameraMatrix = glm::mat4(1.0f);

	bool _cameraChange = true;

};