#pragma once
#include "ICamera.h"


class OrthoCamera : public ICamera {
public:


	OrthoCamera()
	{
		_scale = 1.0f;
		eyePos = glm::vec3(0.f, 0.f, -770.0f);
		aimPos = glm::vec3(0.f, 0.f, 0.f);
	}
	~OrthoCamera()
	{

	}

	void init() override {

		_projectionMatrix = glm::ortho(0.0f, (float)_screenWidth, 0.0f, (float)_screenHeight);

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


	//getters

	glm::ivec2 getCameraDimensions() const override {
		glm::vec2 cameraDimensions = { _screenWidth, _screenHeight };
		return cameraDimensions;
	}

	SDL_FRect getCameraRect() const override {
		float cameraWidth = getCameraDimensions().x / getScale();
		float cameraHeight = getCameraDimensions().y / getScale();

		float cameraX = eyePos.x - cameraWidth / 2.0f + getCameraDimensions().x / 2;
		float cameraY = eyePos.y - cameraHeight / 2.0f + getCameraDimensions().y / 2;

		SDL_FRect cameraRect = { cameraX , cameraY , cameraWidth, cameraHeight };
		return cameraRect;
	}

	void setCameraMatrix(glm::mat4 newMatrix) {
		_cameraChange = true;
	}

};