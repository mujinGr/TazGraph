#pragma once

#include "../../Components.h"

class Line_w_Color : public Component
{
public:
	Color src_color = { 255, 255, 255, 255 };
	Color dest_color = { 255, 255, 255, 255 };


	Line_w_Color()
	{

	}

	~Line_w_Color() {

	}

	void init() override {
	}

	void update(float deltaTime) override {
	}

	void draw(LineRenderer& batch, TazGraphEngine::Window& window) override {
		//float tempScreenScale = window.getScale();

		glm::vec2 fromNodeCenter = entity->getFromPort()->GetComponent<TransformComponent>().getPosition();
		glm::vec2 toNodeCenter = entity->getToPort()->GetComponent<TransformComponent>().getPosition();

		batch.drawLine(fromNodeCenter, toNodeCenter, src_color, dest_color, 0.0f);
	}
};