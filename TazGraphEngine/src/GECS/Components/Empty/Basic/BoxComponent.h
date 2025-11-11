#pragma once

#include "../../../Components.h"

class BoxComponent : public Component
{
public:
	TazColor color = { 255, 255, 255, 255 };

	TransformComponent* transform = nullptr;

	float temp_rotation = 0.0f;

	BoxComponent()
	{

	}


	~BoxComponent() {

	}

	void init() override {
		if (transform == nullptr) {
			entity->addComponent<TransformComponent>();
		}
		transform = &entity->GetComponent<TransformComponent>();

	}

	void update(float deltaTime) override {
		temp_rotation += 0.1f;
	}

	void draw(size_t v_index, PlaneColorRenderer& batch, TazGraphEngine::Window& window) {
		batch.drawBox(v_index, transform->size, transform->position, transform->rotation, color);
	}

	void draw(size_t v_index, LightRenderer& batch, TazGraphEngine::Window& window) {
		batch.drawBox(v_index, transform->size, transform->position, transform->rotation, color);
	}

	std::string GetComponentName() override {
		return "BoxComponent";
	}
};