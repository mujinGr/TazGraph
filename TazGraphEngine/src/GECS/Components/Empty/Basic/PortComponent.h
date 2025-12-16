#pragma once

#include "../../../Components.h"

class PortSlotComponent;
// todo this can be generally a flexbox
class PortComponent : public Component
{
private:
	bool tempIsVertical = false;
public:
	TazColor color = { 255, 255, 255, 255 };


	TransformComponent* transform = nullptr;

	PortComponent()
	{

	}

	PortComponent(bool m_isVertical)
	{
		tempIsVertical = m_isVertical;
	}

	~PortComponent() {

	}

	void init() override {
		transform = &entity->GetComponent<TransformComponent>();
		entity->isVertical = tempIsVertical;
	}

	void update(float deltaTime) override {
		transform->size.x = entity->getParentEntity()->GetComponent<TransformComponent>().size.x;
		transform->size.y = entity->getParentEntity()->GetComponent<TransformComponent>().size.y;
		size_t childrenSize = entity->children.size();
		if (!entity->isVertical) {
			if (childrenSize > 1)
				entity->slotSpacing = transform->size.x / childrenSize;
			else
				entity->slotSpacing = transform->size.x;
		}
		else { // Vertical
			if (childrenSize > 1)
				entity->slotSpacing = transform->size.y / childrenSize;
			else
				entity->slotSpacing = transform->size.y;
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