#pragma once

#include <unordered_map>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

#include <SDL2/SDL.h>

#include "../Camera2.5D/ICamera.h"
class InputManager {
public:
    InputManager();
    ~InputManager();

    void update();

    void pressKey(unsigned int keyID);
    void releaseKey(unsigned int keyID);

    //returns true if the key is held down
    bool isKeyDown(unsigned int keyID);

    //returns true if the key was just pressed
    bool isKeyPressed(unsigned int keyID);

    bool checkMouseCollision(glm::vec2 position, glm::ivec2 tr_size);
    void setMouseCoords(float x, float y);

    glm::vec2 getMouseCoords() const;

    // Panning
    void setPanningPoint(glm::vec2 position);
    glm::vec2 calculatePanningDelta(glm::vec2 position);

    void setObjectRelativePos(glm::vec2 relativeObjectPos);
    glm::vec2 getObjectRelativePos();

    glm::vec2 convertWindowToCameraCoords(glm::vec2 mousePos,
        glm::vec2 viewportSize,
        glm::vec2 windowDimensions,
        const glm::vec2& windowPos, const glm::vec2& windowSize,
        const ICamera& camera);

    /*glm::vec2 convertCameraToWindowCoords(glm::vec2 mousePos,
        glm::vec2 viewportSize,
        glm::vec2 windowDimensions,
        const glm::vec2& windowPos, const glm::vec2& windowSize,
        const ICamera& camera);*/


private:
    bool wasKeyDown(unsigned int keyID);

    std::unordered_map<unsigned int, bool> _keyMap;
    std::unordered_map<unsigned int, bool> _prevKeyMap;

    glm::vec2 _mouseCoords = glm::vec2(0);

    glm::vec2 _panningPoint = glm::vec2(0);

    glm::vec2 _relativeObjectPos = glm::vec2(0);
};
