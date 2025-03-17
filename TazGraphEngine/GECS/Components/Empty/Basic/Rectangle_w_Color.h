#pragma once

#include "../../../Components.h"

class Rectangle_w_Color : public Component
{
public:
	Color color = { 255, 255, 255, 255 };

	SDL_Rect destRect = {0,0,0,0};
	TransformComponent* transform = nullptr;

	float temp_rotation = 0.0f;

	Rectangle_w_Color()
	{

	}


	~Rectangle_w_Color() {

	}

	void init() override {
		transform = &entity->GetComponent<TransformComponent>();

		destRect.w = transform->bodyDims.w * transform->scale;
		destRect.h = transform->bodyDims.h * transform->scale;
	}

	void update(float deltaTime) override {
		destRect.x = static_cast<int>(transform->getPosition().x); //make player move with the camera, being stable in centre, except on edges
		destRect.y = static_cast<int>(transform->getPosition().y);
		destRect.w = transform->bodyDims.w * transform->scale;
		destRect.h = transform->bodyDims.h * transform->scale;

		temp_rotation += 0.1f;
	}

	void draw(PlaneModelRenderer&  batch, TazGraphEngine::Window& window) override {
		glm::vec4 pos((float)destRect.x, (float)destRect.y, (float)destRect.w, (float)destRect.h);
		batch.draw(pos, glm::vec4(-1.0f, -1.0f, 2.0f, 2.0f), 0, transform->getZIndex(), color); // 0 is for texture
	}

	void draw(size_t v_index, PlaneColorRenderer& batch, TazGraphEngine::Window& window) {
		glm::vec4 pos((float)destRect.x, (float)destRect.y, (float)destRect.w, (float)destRect.h);
		batch.draw(v_index, pos, transform->getZIndex(), color);
	}
};