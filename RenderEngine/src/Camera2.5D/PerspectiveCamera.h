#pragma once
#include "ICamera.h"


class PerspectiveCamera : public ICamera {
public:
	glm::vec3 panningAimPos{ 0,0,0 };

	float fov = 45.0f;
	float aspect = 0.0f;
	float nearPlane = 0.1f;

	PerspectiveCamera()
	{
		_scale = 1.0f;
		eyePos = glm::vec3(0.f, 0.f, -770.0f);
		aimPos = glm::vec3(0.f, 0.f, 0.f);
	}

	~PerspectiveCamera()
	{

	}

	void init() override {
		aspect = (float)_screenWidth / (float)_screenHeight;
		_projectionMatrix = glm::perspective(glm::radians(fov), aspect, nearPlane, zFar); //left, right, top, bottom
		updateCameraOrientation();

		_cameraMatrix = glm::mat4(1.0f);

		_cameraMatrix = _projectionMatrix * _viewMatrix;

	}

	void update() override {
		if (_cameraChange) {
			updateCameraOrientation();

			_cameraMatrix = glm::mat4(1.0f);

			_cameraMatrix = _projectionMatrix * _viewMatrix;

		}

	}

	glm::vec3 getEulerAnglesFromDirection(glm::vec3 direction) {
		float yaw = glm::atan(direction.x, direction.z);
		float pitch = glm::asin(-direction.y);
		float roll = 0.0f;

		return glm::vec3(glm::degrees(pitch), glm::degrees(yaw), glm::degrees(roll));
	}


	float getZFar() {
		return zFar;
	}

	void setPanningAimPos(const glm::vec3 newAimPos) {
		panningAimPos = newAimPos;
	}

	glm::vec3 getPanningAimPos() {
		return panningAimPos;
	}

	//getters

	glm::ivec2 getCameraDimensions() const override {
		glm::vec2 cameraDimensions = { _screenWidth, _screenHeight };
		return cameraDimensions;
	}

	SDL_FRect getCameraRect() const override {
		float cameraWidth = getCameraDimensions().x / getScale();
		float cameraHeight = getCameraDimensions().y / getScale();

		float cameraX = eyePos.x - cameraWidth / 2.0f;
		float cameraY = eyePos.y - cameraHeight / 2.0f;

		SDL_FRect cameraRect = { cameraX , cameraY , cameraWidth, cameraHeight };
		return cameraRect;
	}

	void setCameraMatrix(glm::mat4 newMatrix) {
		_cameraChange = true;
	}
	void resetCameraPosition() {

		_scale = 1.0f;

		eyePos = glm::vec3(0.f, 0.f, -770.0f);
		aimPos = glm::vec3(0, 0, 0);

		currentViewMode = ViewMode::Y_UP;
		upDir = glm::vec3(0, -1, 0);

		init();

		_cameraChange = true;
	}
	float getMinScale() {
		return _minScale;
	}

	float getMaxScale() {
		return _maxScale;
	}

	void setAspect(float newAspectX, float newAspectY)
	{
		aspect = newAspectX/ newAspectY;
		_projectionMatrix = glm::perspective(
			glm::radians(fov),
			aspect,
			nearPlane,
			zFar
		);

		updateCameraOrientation();

		_cameraMatrix = glm::mat4(1.0f);

		_cameraMatrix = _projectionMatrix * _viewMatrix;
	}

	float _minScale = 0.1f, _maxScale = 5.0f;

};