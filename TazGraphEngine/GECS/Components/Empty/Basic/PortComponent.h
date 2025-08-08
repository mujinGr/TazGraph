#pragma once

#include "../../../Components.h"

class PortComponent : public Component
{
public:
	Color color = { 255, 255, 255, 255 };

	std::vector<EmptyEntity*> portSlots;

	TransformComponent* transform = nullptr;

	PortComponent()
	{

	}


	~PortComponent() {

	}

	void init() override {
		transform = &entity->GetComponent<TransformComponent>();
	}

	void update(float deltaTime) override {

		//transform->setRotation(transform->getRotation() + 0.1f);
	}

	void draw(size_t v_index, PlaneColorRenderer& batch, TazGraphEngine::Window& window) {
		glm::vec2 size(1, 1);
		batch.draw(v_index, size, transform->bodyCenter, transform->rotation, color);
	}

	std::string GetComponentName() override {
		return "PortComponent";
	}

	void showGUI() override {
		ImGui::Separator();

		ImVec4 a_color = ImVec4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
		if (ImGui::ColorPicker4("Color", (float*)&a_color)) {
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