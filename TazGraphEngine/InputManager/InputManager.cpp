#include "InputManager.h"

InputManager::InputManager() : _mouseCoords(0.0f) {

}

InputManager::~InputManager() {

}

void InputManager::update() {
    //Loop through keyMap and copy to prevKeyMap
    for (auto& it : _keyMap) {
        _prevKeyMap[it.first] = it.second;
    }
}

void InputManager::pressKey(unsigned int keyID) {
    _keyMap[keyID] = true;
}

void InputManager::releaseKey(unsigned int keyID) {
    _keyMap[keyID] = false;
}

bool InputManager::isKeyDown(unsigned int keyID) {
    auto it = _keyMap.find(keyID);
    if (it != _keyMap.end()) {
        return it->second;
    }
    else {
        return false;
    }
}

bool InputManager::isKeyPressed(unsigned int keyID) {
    if ((isKeyDown(keyID) == true) && (wasKeyDown(keyID) == false)) {
        return true;
    }
    return false;
}

bool InputManager::wasKeyDown(unsigned int keyID) {
    auto it = _prevKeyMap.find(keyID);
    if (it != _prevKeyMap.end()) {
        return it->second;
    }
    else {
        return false;
    }
}

bool InputManager::checkMouseCollision(glm::vec2 position, glm::ivec2 tr_size) {
    if (_mouseCoords.x > position.x && _mouseCoords.x < position.x + tr_size.x &&
        _mouseCoords.y > position.y && _mouseCoords.y < position.y + tr_size.y) {
        return true;
    }
    return false;
}

void InputManager::setMouseCoords(float x, float y) {
    _mouseCoords.x = x;
    _mouseCoords.y = y;
}

glm::vec2 InputManager::getMouseCoords() const {
    return _mouseCoords;
}

void InputManager::setStartDragPos(glm::vec2 position)
{
    _startingDragPoint = position;
}

void InputManager::setPanningPoint(glm::vec2 position)
{
    _panningPoint = position;
}

glm::vec2 InputManager::calculatePanningDelta(glm::vec2 position)
{
    glm::vec2 deltaMotion = position - _panningPoint;
    return deltaMotion;
}
