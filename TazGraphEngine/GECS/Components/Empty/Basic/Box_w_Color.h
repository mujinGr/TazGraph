#pragma once

#include "../../../Components.h"

class Box_w_Color : public Component
{
public:
	Color color = { 255, 255, 255, 255 };

	SDL_Rect destRect;
	TransformComponent* transform = nullptr;

	float temp_rotation = 0.0f;

	Box_w_Color()
	{

	}


	~Box_w_Color() {

	}

	void init() override {
		transform = &entity->GetComponent<TransformComponent>();

		destRect.w = transform->size.x * transform->scale;
		destRect.h = transform->size.y * transform->scale;
	}

	void update(float deltaTime) override {
		destRect.x = static_cast<int>(transform->getPosition().x); //make player move with the camera, being stable in centre, except on edges
		destRect.y = static_cast<int>(transform->getPosition().y);
		destRect.w = transform->size.x * transform->scale;
		destRect.h = transform->size.y * transform->scale;

		temp_rotation += 0.1f;
	}

	void draw(size_t v_index, PlaneColorRenderer& batch, TazGraphEngine::Window& window ) {
		batch.drawBox(v_index, transform->size, transform->bodyCenter, transform->rotation,color);
	}

	std::string GetComponentName() override {
		return "Box_w_Color";
	}
};