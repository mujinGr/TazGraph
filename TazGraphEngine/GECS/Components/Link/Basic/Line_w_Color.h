#pragma once

#include "../../../Components.h"

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

		glm::vec3 fromNodeCenter = entity->getFromPort()->GetComponent<TransformComponent>().getPosition();
		glm::vec3 toNodeCenter = entity->getToPort()->GetComponent<TransformComponent>().getPosition();

		batch.drawLine(fromNodeCenter, toNodeCenter, src_color, dest_color);
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
		float t = this->flash_animation.interpolation_a;
		
		if (t < 0.33f) {
			this->src_color = Color::fromVec4(glm::mix(default_src_color.toVec4(), this->flash_animation.flashColor.toVec4(), 3 * t));
		}
		else if (t < 0.66f) {
			this->src_color = Color::fromVec4(glm::mix(this->flash_animation.flashColor.toVec4(), default_src_color.toVec4(), std::min((3 * (t - 0.33f)), 1.0f)));
			this->dest_color = Color::fromVec4(glm::mix(default_dest_color.toVec4(), this->flash_animation.flashColor.toVec4(), 3 * (t - 0.33f)));
		}
		else {
			this->dest_color = Color::fromVec4(glm::mix(this->flash_animation.flashColor.toVec4(), default_dest_color.toVec4(), std::min((3 * (t - 0.66f)), 1.0f)));
		}
		// Smooth transition using lerp (linear interpolation)
		
	}
};