#pragma once

#include "../../pch.h"

enum class ViewMode {
	Y_UP,
	Z_UP
};

class ICamera {
public:
	int _screenWidth = 800, _screenHeight = 640;

	glm::vec3 eyePos{ 0,0,0 };
	glm::vec3 aimPos{ 0,0,0 };
	glm::vec3 upDir{ 0,-1,0 };
	float zFar = 1000000.0f;

	ViewMode currentViewMode = ViewMode::Y_UP;

	virtual ~ICamera() = default;

	// Initializes the camera2D.worldLocation with the screen's width and height
	virtual void init() = 0;

	// Updates the camera2D.worldLocation's matrix if there have been any changes
	virtual void update() = 0;

	// Converts screen coordinates to world coordinates
	glm::vec3 convertScreenToWorldPlane(glm::vec2 screenCoords, float depth = 0.0f);

	// Returns the dimensions of the camera2D.worldLocation's view
	virtual glm::ivec2 getCameraDimensions() const = 0;

	// Returns the SDL_Rect representing the camera2D.worldLocation's viewport
	virtual SDL_FRect getCameraRect() const = 0;

	glm::vec3 convertScreenToWorldDistance(glm::vec2 screenCoords, float depth);

	// Additional methods to expose camera2D.worldLocation properties as needed
	glm::vec3 getPosition() const;
	void setPosition(const glm::vec3 newPosition);
	void setPosition_X(const float newPosition);
	void setPosition_Y(const float newPosition);
	void setPosition_Z(const float newPosition);
	float getScale() const;
	glm::mat4 getCameraMatrix() const;
	glm::vec3 getAimPos();
	void setScale(float scale);

	void makeCameraDirty();
	bool hasChanged();
	void refreshCamera();

	void updateCameraOrientation();

	void setOrientation(glm::vec3 eye, glm::vec3 target, glm::vec3 up);

	bool isPointInCameraView(const glm::vec4 point, float margin);

	// Function to cast a ray from screen coordinates into world space
	glm::vec3 castRayAt(const glm::vec2& screenPos);

	glm::vec3 getPointOnRayAtZ(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float desiredZ);

	void setViewMatrix(glm::mat4 newViewMatrix);

	glm::mat4 getViewMatrix();

	void setProjMatrix(glm::mat4 newProjMatrix);

	glm::mat4 getProjMatrix();

	glm::vec3 getForwardDir();
	glm::vec3 getRightDir();
	glm::vec3 getUpDir();

	void movePosition_Hor(const float step);
	void movePosition_Vert(const float step);
	void movePosition_Forward(const float step);
	void setAimPos(const glm::vec3 newAimPos);
	void moveAimPos(glm::vec3 startingAimPos, const glm::vec2 distance);


protected:
	glm::mat4 _projectionMatrix = glm::mat4(1.0f); // changed once in init
	glm::mat4 _viewMatrix = glm::mat4(1.0f);
	glm::mat4 _cameraMatrix = glm::mat4(1.0f);

	bool _cameraChange = true;
	float _scale; // decreases when zoom-out

};