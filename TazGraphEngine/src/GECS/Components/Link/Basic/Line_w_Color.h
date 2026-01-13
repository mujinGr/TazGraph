#pragma once

#include "../../../Components.h"

class Line_w_Color : public LinkComponent
{
public:
	TazColor default_src_color = { 255, 255, 255, 255 };
	TazColor src_color = { 255, 255, 255, 255 };

	TazColor default_dest_color = { 255, 255, 255, 255 };
	TazColor dest_color = { 255, 255, 255, 255 };

	FlashAnimation flash_animation;

	float width = 5.0f;

	Line_w_Color()
	{

	}

	~Line_w_Color() {

	}

	void init() override {
		entity->setConnectionType(entity->type);
		entity->updateConnection();
	}

	void update(float deltaTime) override {
	}

	void draw(size_t v_index, LineRenderer& batch, TazGraphEngine::Window& window) {
		//float tempScreenScale = window.getScale();
		batch.drawLine(v_index, entity->fromPos, entity->toPos, src_color, dest_color, width);
	}

	void setSrcColor(TazColor clr) {
		default_src_color = clr;
		src_color = clr;
	}

	void setDestColor(TazColor clr) {
		default_dest_color = clr;
		dest_color = clr;
	}

	void SetFlashAnimation(size_t fr, float sp, const Animation::animType type, const std::vector<float>& flashTimes, TazColor flashC, int reps = 0)
	{
		flash_animation = FlashAnimation(fr, sp, type, flashTimes, flashC, reps);
	}

	void setFlashFrame() {
		float t = this->flash_animation.interpolation_a;

		if (t < 0.33f) {
			this->src_color = TazColor::fromVec4(glm::mix(default_src_color.toVec4(), this->flash_animation.flashColor.toVec4(), 3 * t));
		}
		else if (t < 0.66f) {
			this->src_color = TazColor::fromVec4(glm::mix(this->flash_animation.flashColor.toVec4(), default_src_color.toVec4(), std::min((3 * (t - 0.33f)), 1.0f)));
			this->dest_color = TazColor::fromVec4(glm::mix(default_dest_color.toVec4(), this->flash_animation.flashColor.toVec4(), 3 * (t - 0.33f)));
		}
		else {
			this->dest_color = TazColor::fromVec4(glm::mix(this->flash_animation.flashColor.toVec4(), default_dest_color.toVec4(), std::min((3 * (t - 0.66f)), 1.0f)));
		}
		// Smooth transition using lerp (linear interpolation)

	}

	std::string GetComponentName() override {
		return "Line_w_Color";
	}

	void showGUI(std::vector<BaseComponent*> otherComponents = {}) override {
		showGUI(otherComponents, { entity });
	}

	void showGUI(std::vector<BaseComponent*> otherComponents, std::vector<Entity*> otherEntities) override {
		ImGui::Separator();

		ImVec4 a_color = ImVec4(src_color.r / 255.0f, src_color.g / 255.0f, src_color.b / 255.0f, src_color.a / 255.0f);
		if (ImGui::ColorPicker4("TazColor Line Src", (float*)&a_color)) {
			TazColor newColor = {
					   (GLubyte)(a_color.x * 255),
					   (GLubyte)(a_color.y * 255),
					   (GLubyte)(a_color.z * 255),
					   (GLubyte)(a_color.w * 255)
			};

			setSrcColor(newColor);
		}


		ImVec4 b_color = ImVec4(dest_color.r / 255.0f, dest_color.g / 255.0f, dest_color.b / 255.0f, dest_color.a / 255.0f);
		if (ImGui::ColorPicker4("TazColor Line Dest", (float*)&b_color)) {
			TazColor newColor = {
					   (GLubyte)(b_color.x * 255),
					   (GLubyte)(b_color.y * 255),
					   (GLubyte)(b_color.z * 255),
					   (GLubyte)(b_color.w * 255)
			};

			setDestColor(newColor);
		}

		ImGui::SliderFloat("Width: ", &width, 0.0f, 10.0f);
	};
};