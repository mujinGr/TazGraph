#pragma once

#include "../../../Components.h"

class Triangle_w_Color : public Component
{
public:
	Color color = { 255, 255, 255, 255 };

	glm::vec3 v1 = glm::vec3(0), v2 = glm::vec3(0), v3 = glm::vec3(0);
	glm::vec2 uv1 = glm::vec2(0), uv2 = glm::vec2(0), uv3 = glm::vec2(0);

	TransformComponent* transform = nullptr;

	Triangle_w_Color()
	{

	}


	~Triangle_w_Color() {

	}

	void init() override {
		transform = &entity->GetComponent<TransformComponent>();
	}

	void update(float deltaTime) override {
		v1.x = transform->getPosition().x + (transform->bodyDims.w / 2.0f); //make player move with the camera, being stable in centre, except on edges
		v1.y = transform->getPosition().y;
		v1.z = transform->getPosition().z;

		
		v2.x = transform->getPosition().x; //make player move with the camera, being stable in centre, except on edges
		v2.y = transform->getPosition().y + transform->bodyDims.h;
		v2.z = transform->getPosition().z;


		v3.x = transform->getPosition().x + transform->bodyDims.w; //make player move with the camera, being stable in centre, except on edges
		v3.y = transform->getPosition().y + transform->bodyDims.h;
		v3.z = transform->getPosition().z;

		//transform->setRotation(transform->getRotation() + 0.1f);
	}

	void draw(size_t v_index, PlaneModelRenderer& batch, TazGraphEngine::Window& window) {
		float tempScreenScale = window.getScale();

		batch.drawTriangle(
			v_index,
			v1, v2, v3,
			uv1, uv2, uv3,
			0, color
		);
	}

	void draw(size_t v_index, PlaneColorRenderer& batch, TazGraphEngine::Window& window) {
		float tempScreenScale = window.getScale();

		batch.drawTriangle(
			v_index,
			v1, v2, v3,
			transform->getZIndex(), color
		);
	}
	std::string GetComponentName() override {
		return "Triangle_w_Color";
	}
};