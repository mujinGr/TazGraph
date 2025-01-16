#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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

	}

	void update() override {

		if (_cameraChange) {
			_viewMatrix = glm::lookAt(eyePos, //< eye position
				aimPos,  //< aim position
				upDir); //< up direction


			_cameraMatrix = glm::mat4(1.0f);

			glm::vec3 scale(_scale, _scale, 1.0f);
			_cameraMatrix = glm::scale(_cameraMatrix, scale);


			glm::vec3 translate(-_position.x, -_position.y, 0.0f);
			_cameraMatrix = glm::translate(_cameraMatrix, translate); //if glm ortho = -1,1,-1,1 then 1 horizontal with -400,-320 to bottom-left
			
			_cameraMatrix = _projectionMatrix * _viewMatrix * _cameraMatrix;

			//_cameraMatrix = glm::scale(_cameraMatrix, scale);
			_cameraChange = false;
		}
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
		_position.x = newPosition;
		_cameraChange = true;
	}

	void setPosition_Y(const float newPosition) override {
		_position.y = newPosition;
		_cameraChange = true;
	}

	void setScale(float newScale) override {
		_scale = newScale;
		_cameraChange = true;
	}

	//getters
	glm::vec2 getPosition() const override {
		return _position;
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
		_cameraChange = true;
	}

private:
	int _screenWidth, _screenHeight;
	float _scale; // decreases when zoom-out
	bool _cameraChange;

	glm::vec2 _position;
	glm::mat4 _projectionMatrix; // changed once in init
	glm::mat4 _viewMatrix;
	glm::mat4 _cameraMatrix;
};