#pragma once

#include "../../../Components.h"

class BoxComponent : public Component
{
public:
	TazColor color = { 255, 255, 255, 255 };

	TransformComponent* transform = nullptr;

	float temp_rotation = 0.0f;

	BoxComponent()
	{

	}


	~BoxComponent() {

	}

	void init() override {
		if (!entity->hasComponent<TransformComponent>()) {
			entity->addComponent<TransformComponent>();
		}
		transform = &entity->GetComponent<TransformComponent>();

	}

	void update(float deltaTime) override {
		temp_rotation += 0.1f;
	}

	void draw(size_t v_index, PlaneColorRenderer& batch, TazGraphEngine::Window& window) {
		batch.drawBox(v_index, transform->size, transform->position, transform->rotation, color);
	}

	void draw(size_t v_index, LineRenderer& batch, TazGraphEngine::Window& window) {
		batch.drawBox(v_index, transform->size, transform->position, color, transform->rotation, 5.0f);
	}

	void draw(size_t v_index, LightRenderer& batch, TazGraphEngine::Window& window) {
		batch.drawBox(v_index, transform->size, transform->position, transform->rotation, color);
	}

	std::string GetComponentName() override {
		return "BoxComponent";
	}

	void showGUI(std::vector<BaseComponent*> otherComponents = {}) override {
		showGUI(otherComponents, { entity });

	}

	void showGUI(std::vector<BaseComponent*> otherComponents, std::vector<Entity*> otherEntities) override {
		ImGui::Separator();

		ImVec4 a_color = ImVec4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
		if (ImGui::ColorPicker4("TazColor", (float*)&a_color)) {
			TazColor newColor = {
					   (GLubyte)(a_color.x * 255),
					   (GLubyte)(a_color.y * 255),
					   (GLubyte)(a_color.z * 255),
					   (GLubyte)(a_color.w * 255)
			};

			for (auto& e : otherEntities) {
				if (e->hasComponent<BoxComponent>()) {
					e->GetComponent<BoxComponent>().color = newColor;
				}
			}
		}
	};

};