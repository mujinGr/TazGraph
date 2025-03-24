#pragma once

#include "../../../Components.h"

class TransformComponent : public Component //transform as in graphics, we have rotation and scale
{
private:
	layer _layer = 0;
	float _zIndexF = 0;

	// todo remove rotation
	glm::vec3 _rotation = { 0.0f,0.0f,0.0f };
	
	glm::vec2 _velocity = glm::vec2(0.0f);
public:
	SDL_FRect bodyDims = { 0,0,32,32 };
	SDL_FRect last_bodyDims = { 0,0,32,32 };

	glm::vec3 bodyCenter = { 0.0f,0.0f,0.0f };

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
		bodyCenter = getPosition() + getSizeCenter();
	}

	TransformComponent(glm::vec2 position, layer layer , glm::vec2 size, float sc) : TransformComponent(position){
		bodyDims = { position.x, position.y, size.x,size.y };
		_layer = layer;
		scale = sc;
		bodyCenter = getPosition() + getSizeCenter();
	}

	TransformComponent(glm::vec2 position, layer layer, glm::vec2 size, float sc, int sp) : TransformComponent(position, layer, size, sc)
	{
		speed = sp;
	}


	void init() override
	{
		_velocity = glm::zero<glm::ivec2>();
		_zIndexF = getLayerDepth(_layer);
	}
	void update(float deltaTime) override
	{

		if (SDL_FRectEquals(&bodyDims, &last_bodyDims)) {
			return;
		}

		if (!SDL_FRectEquals(&last_bodyDims, &bodyDims) ) {
			bodyCenter = getPosition() + getSizeCenter();
		}

		last_bodyDims = bodyDims;

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

	glm::vec3 getCenterTransform()
	{
		return bodyCenter;
	}

	void setZIndex(float newZ) {
		_zIndexF = newZ;
	}

	float getZIndex() {
		return _zIndexF;
	}

	glm::vec3 getRotation() {
		return _rotation;
	}

	glm::vec3 getPosition() {
		return glm::vec3(bodyDims.x, bodyDims.y, _zIndexF);
	}

	glm::vec3 getSizeCenter() {
		return glm::vec3(bodyDims.w * scale / 2, bodyDims.h * scale / 2, _zIndexF);
	}

	void setPosition_X(float newPosition_X) {
		bodyDims.x = newPosition_X;
	}
	void setPosition_Y(float newPosition_Y) {
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