#pragma once

#include "../../../Components.h"

// todo this can be generally in flexbox
class PortSlotComponent : public Component
{
public:
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
		if (!parentEntity || !parentEntity->hasComponent<PortComponent>()) {
			return;
		}

		PortComponent& parentPort = parentEntity->GetComponent<PortComponent>();

		// Find our index in the parent's portSlots vector
		int myIndex = parentPort.getSlotIndex(static_cast<EmptyEntity*>(entity));
		if (myIndex == -1) {
			return;
		}

		// Calculate and set our position
		glm::vec3 newPosition = parentPort.getSlotPosition(myIndex);
		transform->bodyCenter = newPosition;
	}

	void draw(size_t v_index, PlaneColorRenderer& batch, TazGraphEngine::Window& window) {
		
	}

	std::string GetComponentName() override {
		return "PortSlotComponent";
	}

	void showGUI() override {
		ImGui::Separator();
	}

};