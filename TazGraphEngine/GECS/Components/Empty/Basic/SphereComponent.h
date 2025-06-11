#pragma once

#include "../../../Components.h"

class SphereComponent : public Component
{
public:
	Color color = { 255, 255, 255, 255 };

	TransformComponent* transform = nullptr;

	SphereComponent()
	{

	}


	~SphereComponent() {

	}

	void init() override {
		transform = &entity->GetComponent<TransformComponent>();
	}

	void update(float deltaTime) override {
	}

	void draw(size_t v_index, PlaneColorRenderer& batch, TazGraphEngine::Window& window) {
		batch.drawSphere(v_index, transform->size, transform->bodyCenter, transform->rotation, color);
	}

	void draw(size_t v_index, LightRenderer& batch, TazGraphEngine::Window& window) {
		batch.drawSphere(v_index, transform->size, transform->bodyCenter, transform->rotation, color);
	}

	std::string GetComponentName() override {
		return "SphereComponent";
	}
};