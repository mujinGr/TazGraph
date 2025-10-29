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