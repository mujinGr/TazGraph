#pragma once

#include "../../../Components.h"

class SphereComponent : public Component
{
public:
	TazColor color = { 255, 255, 255, 255 };

	TransformComponent* transform = nullptr;

	SphereComponent()
	{

	}


	~SphereComponent() {

	}

	void init() override {
		transform = &entity->GetComponent<TransformComponent>();
	}

	void update(float deltaTime) override {
	}

	void draw(size_t v_index, PlaneColorRenderer& batch, TazGraphEngine::Window& window) {
		batch.drawSphere(v_index, transform->size, transform->getPosition(), transform->rotation, color);
	}

	void draw(size_t v_index, LightRenderer& batch, TazGraphEngine::Window& window) {
		batch.drawSphere(v_index, transform->size, transform->getPosition(), transform->rotation, color);
	}

	void draw(size_t v_index, LineRenderer& batch, TazGraphEngine::Window& window) {
		batch.drawSphere(v_index, transform->size, transform->getPosition(), color, transform->rotation, 2.0f);
	}

	std::string GetComponentName() override {
		return "SphereComponent";
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