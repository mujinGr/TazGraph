#pragma once
#include <SDL2/SDL.h>
#include "ICamera.h"

class PerspectiveCamera : public ICamera{
public:
	glm::vec3 eyePos{ 0,0,0 };
	glm::vec3 aimPos{ 0,0,0 };
	glm::vec3 upDir{0,-1,0};


	PerspectiveCamera() : _position(0.0f, 0.0f),
		_cameraMatrix(1.0f),	//I
		_projectionMatrix(1.0f),		//I
		_viewMatrix(1.0f),
		_scale(1.0f),
		_cameraChange(true),
		_screenWidth(800),
		_screenHeight(640)
	{
		eyePos = glm::vec3(0.f, 0.f, -770.0f);
		aimPos = glm::vec3(0.f, 0.f, 0.f);
	}

	PerspectiveCamera(glm::vec3 eye_pos, glm::vec3 aim_pos) : PerspectiveCamera()
	{
		eyePos = eye_pos;
		aimPos = aim_pos;
	}

	~PerspectiveCamera()
	{

	}

	void init() override {
		upDir = glm::vec3(0.f, -1.f, 0.f);
		_projectionMatrix = glm::perspective(glm::radians(45.0f), (float)_screenWidth / (float)_screenHeight, 0.1f, 10000.0f); //left, right, top, bottom
		_viewMatrix = glm::lookAt(eyePos, //< eye position
			aimPos,  //< aim position
			upDir); //< up direction

		_cameraMatrix = glm::mat4(1.0f);

		glm::vec3 scale(_scale, _scale, 1.0f);
		_cameraMatrix = glm::scale(_cameraMatrix, scale);


		glm::vec3 translate(-_position.x, -_position.y, 0.0f);
		_cameraMatrix = glm::translate(_cameraMatrix, translate); //if glm ortho = -1,1,-1,1 then 1 horizontal with -400,-320 to bottom-left

		_cameraMatrix = _projectionMatrix * _viewMatrix * _cameraMatrix;

	}

	void update() override {

		_viewMatrix = glm::lookAt(eyePos, //< eye position
			aimPos,  //< aim position
			upDir); //< up direction


		_cameraMatrix = glm::mat4(1.0f);


		glm::vec3 translate(-_position.x, -_position.y, 0.0f);
		_cameraMatrix = glm::translate(_cameraMatrix, translate); //if glm ortho = -1,1,-1,1 then 1 horizontal with -400,-320 to bottom-left

		glm::vec3 scale(_scale, _scale, 1.0f);
		_cameraMatrix = glm::scale(_cameraMatrix, scale);


		_cameraMatrix = _projectionMatrix * _viewMatrix * _cameraMatrix;

	}

	glm::vec2 convertScreenToWorld(glm::vec2 screenCoords) const override {
		SDL_FRect cameraRect = getCameraRect();

		glm::vec2 worldCoords;

		worldCoords = screenCoords;
		worldCoords /= _scale;

		worldCoords.x = worldCoords.x + cameraRect.x;
		worldCoords.y = worldCoords.y + cameraRect.y;


		return worldCoords;
	}

	//setters
	void setPosition_X(const float newPosition) override {
		eyePos.x = newPosition;
		_cameraChange = true;
	}

	void setPosition_Y(const float newPosition) override {
		eyePos.y = newPosition;
		_cameraChange = true;
	}

	void movePosition_Hor(const float step) {
		glm::vec3 direction = glm::normalize(aimPos - eyePos);  // Get movement direction

		// Calculate the right vector (perpendicular to direction and up)
		glm::vec3 right = glm::normalize(glm::cross(direction, upDir));

		// Move the camera horizontally along the right vector
		eyePos += right * step;
		aimPos += right * step;
		_cameraChange = true;
	}
	void movePosition_Vert(const float step) {
		glm::vec3 direction = glm::normalize(aimPos - eyePos);  // Get movement direction

		// Move the camera horizontally along the right vector
		eyePos += upDir * step;
		aimPos += upDir * step;
		_cameraChange = true;
	}

	void movePosition_Forward(const float step) {
		glm::vec3 direction = glm::normalize(aimPos - eyePos);
		eyePos += direction * step;
		aimPos += direction * step;
		_cameraChange = true;
	}

	void setAimPos(const glm::vec3 newAimPos) {
		aimPos = newAimPos;
		_cameraChange = true;
	}

	void moveAimPos(glm::vec3 startingAimPos, const glm::vec2 distance) {
		aimPos = startingAimPos;
		const float sensitivity = 0.001f;

		float yaw = distance.x * sensitivity;  
		float pitch = distance.y * sensitivity;

		glm::vec3 direction = glm::normalize(aimPos - eyePos);

		direction = glm::rotate(direction, yaw, upDir);

		glm::vec3 right = glm::normalize(glm::cross(direction, upDir));

		direction = glm::rotate(direction, pitch, right);

		// Update the aimPos based on the new direction
		aimPos = eyePos + direction;
		_cameraChange = true;
	}

	glm::vec3 getEulerAnglesFromDirection(glm::vec3 direction) {
		// Yaw (Rotation around Y-axis)
		float yaw = glm::atan(direction.x, direction.z);
		// Pitch (Rotation around X-axis)
		float pitch = glm::asin(-direction.y);
		// Roll is typically 0 unless you want roll adjustments
		float roll = 0.0f;

		return glm::vec3(glm::degrees(pitch), glm::degrees(yaw), glm::degrees(roll));
	}


	glm::vec3 getAimPos() {
		return aimPos;
	}

	void setScale(float newScale) override {
		_scale = newScale;
		_cameraChange = true;
	}

	//getters
	glm::vec3 getPosition() const override {
		return eyePos;
	}

	float getScale() const override {
		return _scale;
	}

	glm::mat4 getCameraMatrix() const override {
		return _cameraMatrix;
	}

	glm::ivec2 getCameraDimensions() const override {
		glm::vec2 cameraDimensions = { _screenWidth, _screenHeight };
		return cameraDimensions;
	}

	SDL_FRect getCameraRect() const override {
		float cameraWidth = getCameraDimensions().x / getScale();
		float cameraHeight = getCameraDimensions().y / getScale();

		float cameraX = _position.x - cameraWidth / 2.0f ;
		float cameraY = _position.y - cameraHeight / 2.0f ;

		SDL_FRect cameraRect = { cameraX , cameraY , cameraWidth, cameraHeight };
		return cameraRect;
	}

	void setCameraMatrix(glm::mat4 newMatrix) {
		_cameraChange = true;
	}
	void resetCameraPosition() {

		_position = glm::vec2(0.0f,0.0f);
		_scale = 1.0f;

		eyePos = glm::vec3(0.f, 0.f, -770.0f);
		aimPos = glm::vec3( 0,0,0 );
		upDir = glm::vec3( 0,-1,0 );

		init();

		_cameraChange = true;
	}
	float getMinScale() {
		return _minScale;
	}

	float getMaxScale() {
		return _maxScale;
	}


	bool isPointInCameraView(const glm::vec4 point)
	{
		glm::mat4 vpMatrix = _cameraMatrix;

		glm::vec4 clipSpacePos = vpMatrix * point;

		if (clipSpacePos.w != 0.0f) {
			clipSpacePos.x /= clipSpacePos.w;
			clipSpacePos.y /= clipSpacePos.w;
			clipSpacePos.z /= clipSpacePos.w;
		}

		// 0.2f is the margin
		if (clipSpacePos.x < -1.2f || clipSpacePos.x > 1.2f) return false;
		if (clipSpacePos.y < -1.2f || clipSpacePos.y > 1.2f) return false;
		if (clipSpacePos.z < -0.2f || clipSpacePos.z > 1.2f) return false;

		return true;
	}

	bool hasChanged() override {
		return _cameraChange;
	}

	void refreshCamera() override {
		_cameraChange = false;
	}

	// Function to cast a ray from screen coordinates into world space
	glm::vec3 castRayAt(const glm::vec2& screenPos) {
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
	glm::vec3 getPointOnRayAtZ(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float desiredZ) {
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

private:
	int _screenWidth, _screenHeight;
	float _minScale = 0.1f, _maxScale = 5.0f;
	float _scale; // decreases when zoom-out
	bool _cameraChange;

	glm::vec2 _position;
	glm::mat4 _projectionMatrix; // changed once in init
	glm::mat4 _viewMatrix;
	glm::mat4 _cameraMatrix;
};