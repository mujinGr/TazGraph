#pragma once

#include "../../../Components.h"

class Line_w_Color : public LinkComponent
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

	void draw(size_t v_index, LineRenderer& batch, TazGraphEngine::Window& window) {
		//float tempScreenScale = window.getScale();

		glm::vec3 fromNodeCenter = entity->getFromNode()->GetComponent<TransformComponent>().getCenterTransform();
		glm::vec3 toNodeCenter = entity->getToNode()->GetComponent<TransformComponent>().getCenterTransform();

		batch.drawLine(v_index, fromNodeCenter, toNodeCenter, src_color, dest_color);
	}
	
	void drawWithPorts(size_t v_index, LineRenderer& batch, TazGraphEngine::Window& window) {
		//float tempScreenScale = window.getScale();

		glm::vec3 fromPortCenter = entity->getFromNode()->children[entity->fromPort]->GetComponent<TransformComponent>().getCenterTransform();
		glm::vec3 toPortCenter = entity->getToNode()->children[entity->toPort]->GetComponent<TransformComponent>().getCenterTransform();

		batch.drawLine(v_index, fromPortCenter, toPortCenter, src_color, dest_color);
	}

	void setSrcColor(Color clr) {
		default_src_color = clr;
		src_color = clr;
	}

	void setDestColor(Color clr) {
		default_dest_color = clr;
		dest_color = clr;
	}

	void SetFlashAnimation(int idX, int idY, size_t fr, float sp, const Animation::animType type, const std::vector<float>& flashTimes, Color flashC, int reps = 0)
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

	std::string GetComponentName() override {
		return "Line_w_Color";
	}

	void showGUI() override {
		ImGui::Separator();

		ImVec4 a_color = ImVec4(src_color.r / 255.0f, src_color.g / 255.0f, src_color.b / 255.0f, src_color.a / 255.0f);
		if (ImGui::ColorPicker4("Color Line Src", (float*)&a_color)) {
			Color newColor = {
					   (GLubyte)(a_color.x * 255),
					   (GLubyte)(a_color.y * 255),
					   (GLubyte)(a_color.z * 255),
					   (GLubyte)(a_color.w * 255)
			};

			setSrcColor(newColor);
		}


		ImVec4 b_color = ImVec4(dest_color.r / 255.0f, dest_color.g / 255.0f, dest_color.b / 255.0f, dest_color.a / 255.0f);
		if (ImGui::ColorPicker4("Color Line Dest", (float*)&b_color)) {
			Color newColor = {
					   (GLubyte)(b_color.x * 255),
					   (GLubyte)(b_color.y * 255),
					   (GLubyte)(b_color.z * 255),
					   (GLubyte)(b_color.w * 255)
			};

			setDestColor(newColor);
		}
	}
};