#pragma once

#include "../../../Components.h"

class Triangle_w_Color : public Component
{
public:
	Color color = { 255, 255, 255, 255 };

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

		//transform->setRotation(transform->getRotation() + 0.1f);
	}

	/*void draw(size_t v_index, PlaneModelRenderer& batch, TazGraphEngine::Window& window) {
		float tempScreenScale = window.getScale();

		batch.drawTriangle(
			v_index,
			transform->getPosition(),
			transform->rotation,
			uv1, uv2, uv3,
			0, color
		);
	}*/

	void draw(size_t v_index, PlaneColorRenderer& batch, TazGraphEngine::Window& window) {
		float tempScreenScale = window.getScale();

		batch.drawTriangle(
			v_index,
			transform->bodyCenter, 
			transform->rotation, color
		);
	}
	std::string GetComponentName() override {
		return "Triangle_w_Color";
	}
};