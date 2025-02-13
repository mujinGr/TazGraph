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

void InputManager::setObjectRelativePos(glm::vec2 relativeObjectPos) 
{
    _relativeObjectPos = relativeObjectPos;
}

glm::vec2 InputManager::getObjectRelativePos()
{
    return _relativeObjectPos;
}

glm::vec2 InputManager::convertWindowToCameraCoords(glm::vec2 mousePos,
    glm::vec2 viewportSize,
    glm::vec2 windowDimensions,
    const glm::vec2& windowPos, const glm::vec2& windowSize,
    const ICamera& camera
) {
    
    glm::vec2 cameraDimensions = camera.getCameraDimensions();

    float scale_CToV_X = viewportSize.x / cameraDimensions.x ;
    float scale_CToV_Y = viewportSize.y / cameraDimensions.y ;

    glm::vec2 interMouseCoord(
        mousePos.x * cameraDimensions.x / windowDimensions.x,
        mousePos.y * cameraDimensions.y / windowDimensions.y);

    mousePos.x = ((interMouseCoord.x * scale_CToV_X) - windowPos.x) * cameraDimensions.x / windowSize.x;
    mousePos.y = ((interMouseCoord.y * scale_CToV_Y) - windowPos.y) * cameraDimensions.y / windowSize.y;
    glm::vec2 resultMousePos(mousePos.x, mousePos.y);
    return resultMousePos;
}

glm::vec2 InputManager::convertCameraToWindowCoords(glm::vec2 mousePos,
    glm::vec2 viewportSize,
    glm::vec2 windowDimensions,
    const glm::vec2& windowPos, const glm::vec2& windowSize,
    const ICamera& camera
) {
    glm::vec2 cameraDimensions = camera.getCameraDimensions();

    // Calculate the scaling factors from camera to viewport
    float scale_CToV_X = viewportSize.x / cameraDimensions.x;
    float scale_CToV_Y = viewportSize.y / cameraDimensions.y;

    // Convert camera coordinates to viewport coordinates
    glm::vec2 viewportPos(
        (mousePos.x / cameraDimensions.x) * viewportSize.x,
        (mousePos.y / cameraDimensions.y) * viewportSize.y);

    // Adjust by window position and scale according to the window dimensions
    glm::vec2 windowPosAdjusted(
        ((viewportPos.x + windowPos.x) / viewportSize.x) * windowSize.x,
        ((viewportPos.y + windowPos.y) / viewportSize.y) * windowSize.y);

    return windowPosAdjusted;
}