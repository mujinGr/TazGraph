#pragma once

#include "../../Components.h"

class Line_w_Color : public Component
{
public:
	Color default_src_color = { 255, 255, 255, 255 };
	Color src_color = { 255, 255, 255, 255 };

	Color default_dest_color = { 255, 255, 255, 255 };
	Color dest_color = { 255, 255, 255, 255 };

	FlashAnimation flash_animation;

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
	
	void setSrcColor(Color clr) {
		default_src_color = clr;
		src_color = clr;
	}

	void setDestColor(Color clr) {
		default_dest_color = clr;
		dest_color = clr;
	}

	void SetFlashAnimation(int idX, int idY, int fr, float sp, const Animation::animType type, const std::vector<float>& flashTimes, Color flashC, int reps = 0)
	{
		flash_animation = FlashAnimation(idX, idY, fr, sp, type, flashTimes, flashC, reps);
	}

	void setFlashFrame() {
		if (this->flash_animation.interpolation_a < 0.5f) {
			this->src_color = this->flash_animation.flashColor * 2 * this->flash_animation.interpolation_a
				+ default_src_color * (1 - 2 * this->flash_animation.interpolation_a);
		}
		else {
			this->dest_color = this->flash_animation.flashColor * 2 * this->flash_animation.interpolation_a
				+ default_dest_color * (1 - 2 * (0.5f - this->flash_animation.interpolation_a));
		}
	}
};