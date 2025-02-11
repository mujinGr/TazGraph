#pragma once

#include "../../Components.h"

class Rectangle_w_Color : public Component
{
public:
	Color color = { 255, 255, 255, 255 };

	SDL_Rect destRect;
	TransformComponent* transform = nullptr;

	Rectangle_w_Color()
	{

	}


	~Rectangle_w_Color() {

	}

	void init() override {
		transform = &entity->GetComponent<TransformComponent>();

		destRect.w = transform->width * transform->scale;
		destRect.h = transform->height * transform->scale;
	}

	void update(float deltaTime) override {
		destRect.x = static_cast<int>(transform->getPosition().x); //make player move with the camera, being stable in centre, except on edges
		destRect.y = static_cast<int>(transform->getPosition().y);
		destRect.w = transform->width * transform->scale;
		destRect.h = transform->height * transform->scale;
	}

	void draw(PlaneModelRenderer&  batch, TazGraphEngine::Window& window) override {
		float tempScreenScale = window.getScale();
		float rotation = transform->getRotation();

		glm::vec4 pos((float)destRect.x * tempScreenScale, (float)destRect.y * tempScreenScale, (float)destRect.w * tempScreenScale, (float)destRect.h * tempScreenScale);
		batch.draw(pos, glm::vec4(-1.0f, -1.0f, 2.0f, 2.0f), 0, transform->getZIndex(), color, rotation);
	}
};