#pragma once

#include "../../../Components.h"

// todo this can be generally a flexbox
class PortComponent : public Component
{
public:
	Color color = { 255, 255, 255, 255 };

	bool isVertical = false;

	std::vector<EmptyEntity*> portSlots;
	float slotSpacing = 0.0f;
	TransformComponent* transform = nullptr;

	PortComponent()
	{

	}

	PortComponent(bool m_isVertical)
	{
		isVertical = m_isVertical;
	}

	~PortComponent() {

	}

	void init() override {
		transform = &entity->GetComponent<TransformComponent>();
	}

	void update(float deltaTime) override {
		transform->size.x = entity->getParentEntity()->GetComponent<TransformComponent>().size.x;
		transform->size.y = entity->getParentEntity()->GetComponent<TransformComponent>().size.y;

		if (!isVertical) {
			if (portSlots.size() > 1)
				slotSpacing = transform->size.x / (portSlots.size());
			else
				slotSpacing = transform->size.x;
		}
		else { // Vertical
			if (portSlots.size() > 1)
				slotSpacing = transform->size.y / (portSlots.size());
			else
				slotSpacing = transform->size.y;
		}

	}

	void draw(size_t v_index, PlaneColorRenderer& batch, TazGraphEngine::Window& window) {
		glm::vec2 size(1, 1);
		batch.draw(v_index, size, transform->getPosition(), transform->rotation, color);
	}

	std::string GetComponentName() override {
		return "PortComponent";
	}

	void showGUI(std::vector<BaseComponent*> otherComponents = {}) override {
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

	glm::vec3 getSlotPosition(size_t slotIndex) const {
		if (slotIndex >= portSlots.size()) {
			TazGraphEngine::ConsoleLogger::error("Port Slot index wrong");
			return transform->getPosition();
		}

		glm::vec3 basePos = transform->getPosition();
		glm::vec3 offset(0.0f);

		if (!isVertical) {
			offset.x = (static_cast<float>(slotIndex) - (portSlots.size() - 1) / 2.0f) * slotSpacing;
		}
		else { // Vertical
			offset.y = (static_cast<float>(slotIndex) - (portSlots.size() - 1) / 2.0f) * slotSpacing;
		}

		return basePos + offset;
	}

	// Helper function to find the index of a specific slot
	int getSlotIndex(EmptyEntity* slot) const {
		for (size_t i = 0; i < portSlots.size(); ++i) {
			if (portSlots[i] == slot) {
				return static_cast<int>(i);
			}
		}
		return -1; // Not found
	}

};