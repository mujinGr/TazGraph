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
		v1.x = 0.0f; //make player move with the camera, being stable in centre, except on edges
		v1.y = -transform->size.y / 2.0f;
		v1.z = 0;

		
		v2.x = -transform->size.x / 2.0f; //make player move with the camera, being stable in centre, except on edges
		v2.y = transform->size.y / 2.0f;
		v2.z = 0;


		v3.x = transform->size.x / 2.0f; //make player move with the camera, being stable in centre, except on edges
		v3.y = transform->size.y / 2.0f;
		v3.z = 0;

		//transform->setRotation(transform->getRotation() + 0.1f);
	}

	void draw(size_t v_index, PlaneModelRenderer& batch, TazGraphEngine::Window& window) {
		float tempScreenScale = window.getScale();

		batch.drawTriangle(
			v_index,
			transform->getPosition(),
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
			transform->bodyCenter, 
			transform->rotation, color
		);
	}
	std::string GetComponentName() override {
		return "Triangle_w_Color";
	}
};