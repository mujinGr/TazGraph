#pragma once

#include "../../../Components.h"

class TransformComponent : public Component //transform as in graphics, we have rotation and scale
{
private:
	layer _layer = 0;
	float _zIndexF = 0;

	// todo remove rotation
	glm::vec3 _rotation = { 0.0f,0.0f,0.0f };
	
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

	TransformComponent(glm::vec2 position, layer layer , glm::vec2 size, float sc) : TransformComponent(position){
		bodyDims = { position.x, position.y, size.x,size.y };
		_layer = layer;
		scale = sc;
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
		return getPosition() + getSizeCenter();
	}

	void setZIndex(float newZ) {
		_zIndexF = newZ;
	}

	float getZIndex() {
		return _zIndexF;
	}

	glm::mat4 setRotation(glm::vec3 newRot) {
		float radX = glm::radians(newRot.x);
		float radY = glm::radians(newRot.y);
		float radZ = glm::radians(newRot.z);

		glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), radX, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), radY, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), radZ, glm::vec3(0.0f, 0.0f, 1.0f));

		glm::mat4 rotationMatrix = rotationZ * rotationY * rotationX;
		return rotationMatrix;
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

	glm::vec2 rotatePoint(float x, float y, float centerX, float centerY, float radians) {
		float dx = x - centerX;
		float dy = y - centerY;
		return glm::vec2(
			centerX + dx * cos(radians) - dy * sin(radians),
			centerY + dx * sin(radians) + dy * cos(radians)
		);
	};
};