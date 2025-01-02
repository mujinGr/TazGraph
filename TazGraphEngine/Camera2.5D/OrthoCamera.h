#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL.h>
#include "ICamera.h"

class OrthoCamera : public ICamera {
public:
	OrthoCamera() : _position(0.0f, 0.0f),
		_cameraMatrix(1.0f),	//I
		_projectionMatrix(1.0f),		//I
		_viewMatrix(1.0f),
		_scale(1.0f),
		_cameraChange(true),
		_screenWidth(800),
		_screenHeight(640)
	{

	}
	~OrthoCamera()
	{

	}

	void init() override {

		_projectionMatrix = glm::ortho(0.0f, (float)_screenWidth, (float)_screenHeight, 0.0f);
	}

	void update() override {

		if (_cameraChange) {

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

		float cameraX = _position.x - cameraWidth / 2.0f + getCameraDimensions().x / 2;
		float cameraY = _position.y - cameraHeight / 2.0f + getCameraDimensions().y / 2;

		SDL_FRect cameraRect = { cameraX , cameraY , cameraWidth, cameraHeight };
		return cameraRect;
	}

	void setCameraMatrix(glm::mat4 newMatrix) {
		_cameraChange = true;
	}


private:
	int _screenWidth, _screenHeight;
	float _scale;
	bool _cameraChange;

	glm::vec2 _position;
	glm::mat4 _projectionMatrix; // changed once in init
	glm::mat4 _viewMatrix;
	glm::mat4 _cameraMatrix;
};