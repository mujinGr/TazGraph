#pragma once

#include <unordered_map>
#include <glm/glm.hpp>

#include <SDL2/SDL.h>
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
    void setStartDragPos(glm::vec2 position); // set the position of camera when starting dragging
    glm::vec2 getStartDragPos() const { return _startingDragPoint;  };

    void setPanningPoint(glm::vec2 position);
    glm::vec2 calculatePanningDelta(glm::vec2 position);
private:
    bool wasKeyDown(unsigned int keyID);

    std::unordered_map<unsigned int, bool> _keyMap;
    std::unordered_map<unsigned int, bool> _prevKeyMap;

    glm::vec2 _mouseCoords;

    glm::vec2 _panningPoint;

    glm::vec2 _startingDragPoint;
};