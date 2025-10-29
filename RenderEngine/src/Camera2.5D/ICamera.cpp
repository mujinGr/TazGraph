#include "./ICamera.h"

glm::vec3 ICamera::convertScreenToWorldPlane(glm::vec2 screenCoords, float depth) {
	glm::vec3 rayOrigin = getPosition();

	// Get ray direction from screen
	glm::vec3 rayDir = castRayAt(screenCoords);

	// Get point where ray hits desired Z
	return getPointOnRayAtZ(rayOrigin, rayDir, depth);
}

glm::vec3 ICamera::convertScreenToWorldDistance(glm::vec2 screenCoords, float distanceFromCamera) {
	glm::vec3 rayOrigin = getPosition();

	// Get ray direction from screen
	glm::vec3 rayDir = castRayAt(screenCoords);

	// Get point where ray hits desired Z
	return rayOrigin + rayDir * distanceFromCamera;
}

glm::vec3 ICamera::getPosition() const
{
	return eyePos;
}

void ICamera::setPosition(const glm::vec3 newPosition)
{
	eyePos = newPosition;
	_cameraChange = true;
}

void ICamera::setPosition_X(const float newPosition)
{
	eyePos.x = newPosition;
	_cameraChange = true;
}

void ICamera::setPosition_Y(const float newPosition)
{
	eyePos.y = newPosition;
	_cameraChange = true;
}

void ICamera::setPosition_Z(const float newPosition)
{
	eyePos.z = newPosition;
	_cameraChange = true;
}

float ICamera::getScale() const
{
	return _scale;
}

glm::mat4 ICamera::getCameraMatrix() const
{
	return _cameraMatrix;
}

glm::vec3 ICamera::getAimPos()
{
	return aimPos;
}

void ICamera::setScale(float scale)
{
	_scale = scale;
	_cameraChange = true;
}

void ICamera::makeCameraDirty()
{
	_cameraChange = true;
}

bool ICamera::hasChanged()
{
	return _cameraChange;
}

void ICamera::refreshCamera()
{
	_cameraChange = false;
}

void ICamera::updateCameraOrientation() {
	if (currentViewMode == ViewMode::Y_UP) {
		upDir = glm::vec3(0.0f, -1.0f, 0.0f);

		setOrientation(
			eyePos, aimPos, upDir
		);
	}
	else {
		upDir = glm::vec3(0.0f, 0.0f, -1.0f);

		setOrientation(
			eyePos, aimPos, upDir
		);
	}
}

void ICamera::setOrientation(glm::vec3 eye, glm::vec3 target, glm::vec3 up) {
	_viewMatrix = glm::lookAt(eye, target, up);
}

bool ICamera::isPointInCameraView(const glm::vec4 point, float margin)
{
	glm::mat4 vpMatrix = _cameraMatrix;

	glm::vec4 clipSpacePos = vpMatrix * point;

	if (clipSpacePos.w != 0.0f) {
		clipSpacePos.x /= clipSpacePos.w;
		clipSpacePos.y /= clipSpacePos.w;
		clipSpacePos.z /= clipSpacePos.w;
	}

	// 0.2f is the margin
	if (clipSpacePos.x < -1.0f - margin || clipSpacePos.x > 1.0f + margin) return false;
	if (clipSpacePos.y < -1.0f - margin || clipSpacePos.y > 1.0f + margin) return false;
	if (clipSpacePos.z < -margin || clipSpacePos.z > 1.0f + margin) return false;

	return true;
}

// Function to cast a ray from screen coordinates into world space
glm::vec3 ICamera::castRayAt(const glm::vec2& screenPos) {
	// Convert screen position to normalized device coordinates (NDC)
	float x = (2.0f * screenPos.x) / _screenWidth - 1.0f;
	float y = 1.0f - (2.0f * screenPos.y) / _screenHeight;
	glm::vec4 clipCoords = glm::vec4(x, y, -1.0f, 1.0f);

	// Convert to eye space
	glm::vec4 eyeCoords = glm::inverse(_projectionMatrix) * clipCoords;
	eyeCoords = glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);

	// Convert to world space
	glm::vec3 worldRay = glm::vec3(glm::inverse(_viewMatrix) * eyeCoords);
	worldRay = glm::normalize(worldRay);

	return worldRay;
}
glm::vec3 ICamera::getPointOnRayAtZ(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float desiredZ) {
	// Check if the ray is parallel to the z-plane (no intersection)
	if (rayDirection.z == 0.0f) {
		// Ray is parallel to the plane, no intersection
		return glm::vec3(std::numeric_limits<float>::infinity()); // Return invalid point
	}

	// Calculate t for the desired z value
	float t = (desiredZ - rayOrigin.z) / rayDirection.z;

	// Calculate the point on the ray
	glm::vec3 pointOnRay = rayOrigin + t * rayDirection;

	return pointOnRay;
}

void ICamera::setViewMatrix(glm::mat4 newViewMatrix) {
	_viewMatrix = newViewMatrix;
	_cameraChange = true;

}

glm::mat4 ICamera::getViewMatrix() {
	return _viewMatrix;
}

void ICamera::setProjMatrix(glm::mat4 newProjMatrix) {
	_projectionMatrix = newProjMatrix;
	_cameraChange = true;

}

glm::mat4 ICamera::getProjMatrix() {
	return _projectionMatrix;
}

glm::vec3 ICamera::getUpDir() {
	return upDir;
}

