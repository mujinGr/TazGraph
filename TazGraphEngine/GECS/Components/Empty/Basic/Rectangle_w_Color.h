#pragma once

#include "../../../Components.h"

class Rectangle_w_Color : public Component
{
public:
	Color color = { 255, 255, 255, 255 };

	SDL_FRect destRect = {0,0,0,0};
	TransformComponent* transform = nullptr;

	float temp_rotation = 0.0f;

	Rectangle_w_Color()
	{

	}


	~Rectangle_w_Color() {

	}

	void init() override {
		transform = &entity->GetComponent<TransformComponent>();

		destRect.w = transform->size.x * transform->scale;
		destRect.h = transform->size.y * transform->scale;
	}

	void update(float deltaTime) override {
		destRect.x = (transform->getPosition().x); //make player move with the camera, being stable in centre, except on edges
		destRect.y = (transform->getPosition().y);
		destRect.w = transform->size.x * transform->scale;
		destRect.h = transform->size.y * transform->scale;

		temp_rotation += 0.1f;
	}

	void draw(size_t v_index, PlaneModelRenderer&  batch, TazGraphEngine::Window& window) {
		glm::vec4 pos((float)destRect.x, (float)destRect.y, (float)destRect.w, (float)destRect.h);
		batch.draw(v_index, pos, glm::vec4(-1.0f, -1.0f, 2.0f, 2.0f), 0, transform->getPosition().z + transform->size.z / 2.0f, color); // 0 is for texture
	}

	void draw(size_t v_index, PlaneColorRenderer& batch, TazGraphEngine::Window& window) {
		glm::vec4 pos((float)destRect.x, (float)destRect.y, (float)destRect.w, (float)destRect.h);
		batch.draw(v_index, pos, transform->getPosition().z + transform->size.z / 2.0f, color);
	}

	std::string GetComponentName() override {
		return "Rectangle_w_Color";
	}

	void showGUI() override {
		ImGui::Separator();

		ImVec4 a_color = ImVec4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
		if(ImGui::ColorPicker4("Color", (float*)&a_color)) {
			Color newColor = {
					   (GLubyte)(a_color.x * 255),
					   (GLubyte)(a_color.y * 255),
					   (GLubyte)(a_color.z * 255),
					   (GLubyte)(a_color.w * 255)
			};

			color = newColor;
		}

	}
};