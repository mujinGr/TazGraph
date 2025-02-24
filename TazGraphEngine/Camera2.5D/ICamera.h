#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <SDL2/SDL.h>

class ICamera {
public:
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
    virtual void setPosition_X(const float newPosition) = 0;
    virtual void setPosition_Y(const float newPosition) = 0;
    virtual float getScale() const = 0;
    virtual glm::mat4 getCameraMatrix() const = 0;
    virtual void setScale(float scale) = 0;

    virtual bool isPointInCameraView(const glm::vec4 point) = 0;
    virtual bool hasChanged() = 0;
};