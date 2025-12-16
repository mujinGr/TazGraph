#pragma once

#include "../../../Components.h"

// todo this can be generally in flexbox
class PortSlotComponent : public Component
{
public:
	int index = -1;

	TransformComponent* transform = nullptr;

	PortSlotComponent()
	{

	}

	~PortSlotComponent() {

	}

	void init() override {
		transform = &entity->GetComponent<TransformComponent>();
	}

	void update(float deltaTime) override {
		Entity* parentEntity = entity->getParentEntity();
		if (!parentEntity) {
			return;
		}

		// Find our index in the parent's portSlots vector
		if (index == -1) {
			return;
		}

		// Calculate and set our position
		glm::vec3 newPosition = getSlotPosition();
		transform->local_position = newPosition;
	}

	glm::vec3 getSlotPosition() const {
		size_t childrenSize = entity->getParentEntity()->children.size();
		if (index >= childrenSize) {
			//TazGraphEngine::ConsoleLogger::error("Port Slot index wrong");
			return entity->getParentEntity()->
				GetComponent<TransformComponent>().getPosition();
		}

		glm::vec3 offset(0.0f);

		if (!entity->getParentEntity()->isVertical) {
			offset.x = (static_cast<float>(index) - (childrenSize - 1) / 2.0f) * entity->getParentEntity()->slotSpacing;
		}
		else { // Vertical
			offset.y = (static_cast<float>(index) - (childrenSize - 1) / 2.0f) * entity->getParentEntity()->slotSpacing;
		}

		return offset;
	}

	void draw(size_t v_index, PlaneColorRenderer& batch, TazGraphEngine::Window& window) {

	}

	std::string GetComponentName() override {
		return "PortSlotComponent";
	}

	void showGUI(std::vector<BaseComponent*> otherComponents = {}) override {
		ImGui::Separator();
	}

};