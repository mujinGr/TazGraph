#pragma once

#include "Components.h"

#include "GOSManager.h"

class TransformComponent : public Component //transform as in graphics, we have rotation and scale
{
private:
	float _zIndex = 0.0f;
	float _rotation = 0.0f;

	glm::vec2 _position;
	glm::vec2 _velocity;
public:
	int height = 32;
	int width = 32;
	float scale = 1;

	int speed = 1;

	bool activatedMovement = false;

	TransformComponent()
	{
		_position = glm::zero<glm::vec2>();
	}

	TransformComponent(float sc)
	{
		_position = glm::zero<glm::vec2>();
		scale = sc;
	}

	TransformComponent(glm::vec2 position)
	{
		_position = position;
	}

	TransformComponent(glm::vec2 position, Layer layer , glm::ivec2 size, float sc) : TransformComponent(position){
		width = size.x;
		height = size.y;
		_zIndex = entity->getLayerValue(layer);
		scale = sc;
	}

	TransformComponent(glm::vec2 position, Layer layer, glm::ivec2 size, float sc, int sp) : TransformComponent(position, layer, size, sc)
	{
		speed = sp;
	}

	
	
	void init() override
	{
		_velocity = glm::zero<glm::ivec2>();
	}
	void update(float deltaTime) override
	{

		std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

		SDL_Rect dimensions = { _position.x, _position.y, width, height };
		SDL_Rect cameraDimensions = main_camera2D->getCameraRect();

		if (entity->checkCollision(dimensions, cameraDimensions)) { //culling
			entity->paused = false;
		}
		else {
			entity->paused = true;
		}
		
		if (entity->getParentEntity() != nullptr) {
			_position.x = entity->getParentEntity()->GetComponent<TransformComponent>().getPosition().x;
			_position.y = entity->getParentEntity()->GetComponent<TransformComponent>().getPosition().y;

			// here i can have specific positioning based on display layout of entity
			// todo: create a UILayout Class for the entities that have inside them other entities that also has a enum 
			// todo: for the layout mode
			_position.x += entity->getParentEntity()->GetComponent<TransformComponent>().getSizeCenter().x 
				- getSizeCenter().x ;
			_position.y += entity->getParentEntity()->GetComponent<TransformComponent>().getSizeCenter().y 
				- getSizeCenter().y ;
		}

		_position.x += _velocity.x * speed * deltaTime;
		float distanceY = _velocity.y * speed * deltaTime;
		if(distanceY < 1)
			_position.y += _velocity.y * speed;
		else
			_position.y += _velocity.y * speed * deltaTime;
	}

	glm::vec2 getCenterTransform()
	{
		return getPosition() + getSizeCenter();
	}

	void setZIndex(float newZ) {
		_zIndex = newZ;
	}

	float getZIndex() {
		return _zIndex;
	}

	void setRotation(float newRot) {
		_rotation = newRot;
	}

	float getRotation() {
		return _rotation;
	}

	glm::vec2 getPosition() {
		return _position;
	}

	glm::vec2 getSizeCenter() {
		return glm::vec2(width * scale / 2,height * scale / 2);
	}

	void setPosition_X(int newPosition_X) {
		_position.x = newPosition_X;
	}
	void setPosition_Y(int newPosition_Y) {
		_position.y = newPosition_Y;
	}

	glm::ivec2 getVelocity() {
		return _velocity;
	}

	void setVelocity_X(float newVelocity_X) {
		_velocity.x = newVelocity_X;
	}
	void setVelocity_Y(float newVelocity_Y) {
		_velocity.y = newVelocity_Y;
	}
};