#pragma once
#include <SDL2/SDL.h>
#include "ICamera.h"


class OrthoCamera : public ICamera {
public:
	glm::vec3 eyePos{ 0,0,0 };
	glm::vec3 aimPos{ 0,0,0 };
	glm::vec3 upDir{ 0,1,0 };
	float zFar = 1000000.0f;

	ViewMode currentViewMode = ViewMode::Y_UP;

	OrthoCamera() : _position(0.0f),
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
	~OrthoCamera()
	{

	}

	void init() override {

		_projectionMatrix = glm::ortho(0.0f, (float)_screenWidth, (float)_screenHeight, 0.0f);

		updateCameraOrientation();

		_cameraMatrix = glm::mat4(1.0f);

		glm::vec3 scale(_scale, _scale, 1.0f);
		_cameraMatrix = glm::scale(_cameraMatrix, scale);


		glm::vec3 translate(-_position.x, -_position.y, 0.0f);
		_cameraMatrix = glm::translate(_cameraMatrix, translate); //if glm ortho = -1,1,-1,1 then 1 horizontal with -400,-320 to bottom-left

		_cameraMatrix = _projectionMatrix * _viewMatrix * _cameraMatrix;
	}

	void update() override {

		if (_cameraChange) {
			updateCameraOrientation();

			_cameraMatrix = glm::mat4(1.0f);

			glm::vec3 scale(_scale, _scale, 1.0f);
			_cameraMatrix = glm::scale(_cameraMatrix, scale);


			glm::vec3 translate(-_position.x, -_position.y, 0.0f);
			_cameraMatrix = glm::translate(_cameraMatrix, translate); //if glm ortho = -1,1,-1,1 then 1 horizontal with -400,-320 to bottom-left

			_cameraMatrix = _projectionMatrix * _viewMatrix * _cameraMatrix;
		}
	}

	void updateCameraOrientation() {
		if (currentViewMode == ViewMode::Y_UP) {
			upDir = glm::vec3(0.0f, 1.0f, 0.0f);

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

	void setOrientation(glm::vec3 eye, glm::vec3 target, glm::vec3 up) {
		_viewMatrix = glm::lookAt(eye, target, up);
	}



	glm::vec2 convertScreenToWorld(glm::vec2 screenCoords) const override {
		//Make 0 the center
		screenCoords -= glm::vec2(_screenWidth / 2, _screenHeight / 2);
		//Scale coordinates
		screenCoords /= _scale;
		screenCoords += glm::vec2(_screenWidth / 2, _screenHeight / 2);
		//Translate with the camera2D.worldLocation position
		screenCoords.x += _position.x;
		screenCoords.y += _position.y;


		return screenCoords;
	}

	//setters
	void setPosition(const glm::vec3 newPosition) override {
		eyePos = newPosition;
		_cameraChange = true;
	}

	void setPosition_X(const float newPosition) override {
		eyePos.x = newPosition;
		_cameraChange = true;
	}

	void setPosition_Y(const float newPosition) override {
		eyePos.y = newPosition;
		_cameraChange = true;
	}

	void setPosition_Z(const float newPosition) override {
		eyePos.z = newPosition;
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
		const float sensitivity = 0.005f;

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
		float yaw = glm::atan(direction.x, direction.z);
		float pitch = glm::asin(-direction.y);
		float roll = 0.0f;

		return glm::vec3(glm::degrees(pitch), glm::degrees(yaw), glm::degrees(roll));
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

		float cameraX = _position.x - cameraWidth / 2.0f + getCameraDimensions().x / 2;
		float cameraY = _position.y - cameraHeight / 2.0f + getCameraDimensions().y / 2;

		SDL_FRect cameraRect = { cameraX , cameraY , cameraWidth, cameraHeight };
		return cameraRect;
	}

	void setCameraMatrix(glm::mat4 newMatrix) {
		_cameraChange = true;
	}

	bool isPointInCameraView(const glm::vec4 point, float margin)
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

	bool hasChanged() override {
		return _cameraChange;
	}

	void makeCameraDirty() override {
		_cameraChange = true;
	}

	void refreshCamera() override {
		_cameraChange = false;
	}

	void setViewMatrix(glm::mat4 newViewMatrix) {
		_viewMatrix = newViewMatrix;
		_cameraChange = true;

	}

	glm::mat4 getViewMatrix() {
		return _viewMatrix;
	}

	void setProjMatrix(glm::mat4 newProjMatrix) {
		_projectionMatrix = newProjMatrix;
		_cameraChange = true;

	}

	glm::mat4 getProjMatrix() {
		return _projectionMatrix;
	}

private:
	int _screenWidth, _screenHeight;
	float _scale;
	bool _cameraChange;

	glm::vec3 _position;
	glm::mat4 _projectionMatrix; // changed once in init
	glm::mat4 _viewMatrix;
	glm::mat4 _cameraMatrix;
};