#pragma once

#include "../../../Components.h"

class Rectangle_w_Color : public Component
{
public:
	TazColor default_color = { 255, 255, 255, 255 };

	TazColor color = { 255, 255, 255, 255 };

	TransformComponent* transform = nullptr;

	FlashAnimation flash_animation;

	Rectangle_w_Color()
	{

	}


	~Rectangle_w_Color() {

	}

	void init() override {
		transform = &entity->GetComponent<TransformComponent>();
	}

	void update(float deltaTime) override {
	}

	//void draw(size_t v_index, PlaneModelRenderer&  batch, TazGraphEngine::Window& window) {
	//	glm::vec3 pos((float)destRect.x, (float)destRect.y, transform->getPosition().z);
	//	glm::vec2 size = glm::vec2((float)destRect.w, (float)destRect.h);
	//	batch.draw(v_index, pos, size, transform->rotation, glm::vec4(-1.0f, -1.0f, 2.0f, 2.0f), 0, transform->getPosition(), color); // 0 is for texture
	//}

	void draw(size_t v_index, PlaneColorRenderer& batch, TazGraphEngine::Window& window) {
		glm::vec2 size((float)transform->size.x, (float)transform->size.y);
		batch.draw(v_index, size, transform->getPosition(), transform->rotation, color);
	}

	void setColor(TazColor clr) {
		default_color = clr;
		color = clr;
	}

	void SetFlashAnimation(int idX, int idY, size_t fr, float sp, const Animation::animType type, const std::vector<float>& flashTimes, TazColor flashC, int reps = 0)
	{
		flash_animation = FlashAnimation(idX, idY, fr, sp, type, flashTimes, flashC, reps);
	}

	void setFlashFrame() {
		float t = this->flash_animation.interpolation_a;
		this->color = TazColor::fromVec4(glm::mix(default_color.toVec4(), this->flash_animation.flashColor.toVec4(), t));
	}


	std::string GetComponentName() override {
		return "Rectangle_w_Color";
	}

	void showGUI(std::vector<BaseComponent*> otherComponents = {}) override {
		ImGui::Separator();

		ImVec4 a_color = ImVec4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
		if (ImGui::ColorPicker4("TazColor", (float*)&a_color)) {
			TazColor newColor = {
					   (GLubyte)(a_color.x * 255),
					   (GLubyte)(a_color.y * 255),
					   (GLubyte)(a_color.z * 255),
					   (GLubyte)(a_color.w * 255)
			};

			color = newColor;
		}

	}
};