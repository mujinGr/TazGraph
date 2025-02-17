#pragma once

#include "../../Components.h"

#include "../../GECSManager.h"

class TransformComponent : public Component //transform as in graphics, we have rotation and scale
{
private:
	std::shared_ptr<PerspectiveCamera> _main_camera2D;
	float _zIndex = 0.0f;
	float _rotation = 0.0f;
	// todo make position as SDL_FRect
	
	glm::vec2 _velocity;
public:
	SDL_FRect bodyDims = { 0,0,32,32 };
	SDL_FRect last_bodyDims = { 0,0,32,32 };

	float scale = 1;

	int speed = 1;

	bool activatedMovement = false;

	TransformComponent()
	{
	}

	TransformComponent(float sc)
	{
		scale = sc;
	}

	TransformComponent(glm::vec2 position)
	{
		bodyDims.x = position.x;
		bodyDims.y = position.y;
	}

	TransformComponent(glm::vec2 position, Layer layer , glm::vec2 size, float sc) : TransformComponent(position){
		bodyDims = { position.x, position.y, size.x,size.y };
		_zIndex = entity->getLayerValue(layer);
		scale = sc;
	}

	TransformComponent(glm::vec2 position, Layer layer, glm::vec2 size, float sc, int sp) : TransformComponent(position, layer, size, sc)
	{
		speed = sp;
	}


	void init() override
	{
		_velocity = glm::zero<glm::ivec2>();
	}
	void update(float deltaTime) override
	{

		if (SDL_FRectEquals(&bodyDims, &last_bodyDims)) {
			return;
		}
		last_bodyDims = bodyDims;

		if (!_main_camera2D) {
			_main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
		}

		SDL_FRect cameraDimensions = _main_camera2D->getCameraRect();
		
		bodyDims.x += _velocity.x * speed * deltaTime;
		bodyDims.y += _velocity.y * speed * deltaTime;
		//todo dont update the children on every iteration
		// todo do this for component when needed
		if (!entity->children.empty()) { // this will not be executed for links since they dont have transformComponent

			if (entity->children["leftPort"]) {
				entity->children["leftPort"]->GetComponent<TransformComponent>().setPosition_X(bodyDims.x);
				entity->children["leftPort"]->GetComponent<TransformComponent>().setPosition_Y(bodyDims.y + bodyDims.h / 2.0f);
			}
			if (entity->children["topPort"]) {
				entity->children["topPort"]->GetComponent<TransformComponent>().setPosition_X(bodyDims.x + bodyDims.w / 2.0f);
				entity->children["topPort"]->GetComponent<TransformComponent>().setPosition_Y(bodyDims.y );
			}
			if (entity->children["rightPort"]) {
				entity->children["rightPort"]->GetComponent<TransformComponent>().setPosition_X(bodyDims.x + bodyDims.w);
				entity->children["rightPort"]->GetComponent<TransformComponent>().setPosition_Y(bodyDims.y + bodyDims.h / 2.0f);
			}
			if (entity->children["bottomPort"]) {
				entity->children["bottomPort"]->GetComponent<TransformComponent>().setPosition_X(bodyDims.x + bodyDims.w / 2.0f);
				entity->children["bottomPort"]->GetComponent<TransformComponent>().setPosition_Y(bodyDims.y + bodyDims.h);
			}
		}
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
		return glm::vec2(bodyDims.x, bodyDims.y);
	}

	glm::vec2 getSizeCenter() {
		return glm::vec2(bodyDims.w * scale / 2, bodyDims.h * scale / 2);
	}

	void setPosition_X(int newPosition_X) {
		bodyDims.x = newPosition_X;
	}
	void setPosition_Y(int newPosition_Y) {
		bodyDims.y = newPosition_Y;
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