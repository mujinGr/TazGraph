#pragma once

#include "../../../Components.h"

class Line_w_Color : public LinkComponent
{
public:
	TazColor default_src_color = { 255, 255, 255, 255 };
	TazColor src_color = { 255, 255, 255, 255 };

	TazColor default_dest_color = { 255, 255, 255, 255 };
	TazColor dest_color = { 255, 255, 255, 255 };

	FlashAnimation flash_animation;

	float width = 5.0f;

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

		glm::vec3 fromNodeCenter = entity->getFromNode()->GetComponent<TransformComponent>().getPosition();
		glm::vec3 toNodeCenter = entity->getToNode()->GetComponent<TransformComponent>().getPosition();

		batch.drawLine(v_index, fromNodeCenter, toNodeCenter, src_color, dest_color, width);
	}

	void drawWithPorts(size_t v_index, LineRenderer& batch, TazGraphEngine::Window& window) {
		//float tempScreenScale = window.getScale();

		if (std::holds_alternative<int>(entity->fromPort) || std::holds_alternative<int>(entity->toPort)) {
			TazGraphEngine::ConsoleLogger::error("Ports are not assigned!");
			return;
		}

		NodeEntity* fromNode = entity->getFromNode();
		NodeEntity* toNode = entity->getToNode();

		Entity* fromPortEntity = fromNode->children[entity->fromPort];
		Entity* toPortEntity = toNode->children[entity->toPort];
		PortComponent& fromPortComp = fromPortEntity->GetComponent<PortComponent>();
		PortComponent& toPortComp = toPortEntity->GetComponent<PortComponent>();

		if ((entity->fromSlotIndex >= fromPortComp.portSlots.size())
			|| (entity->toSlotIndex >= toPortComp.portSlots.size())) {
			return;
		}
		glm::vec3 fromConnectionPoint = fromPortComp.portSlots[entity->fromSlotIndex]->GetComponent<TransformComponent>().getPosition();
		glm::vec3 toConnectionPoint = toPortComp.portSlots[entity->toSlotIndex]->GetComponent<TransformComponent>().getPosition();

		batch.drawLine(v_index, fromConnectionPoint, toConnectionPoint, src_color, dest_color, width);
	}

	void setSrcColor(TazColor clr) {
		default_src_color = clr;
		src_color = clr;
	}

	void setDestColor(TazColor clr) {
		default_dest_color = clr;
		dest_color = clr;
	}

	void SetFlashAnimation(size_t fr, float sp, const Animation::animType type, const std::vector<float>& flashTimes, TazColor flashC, int reps = 0)
	{
		flash_animation = FlashAnimation(fr, sp, type, flashTimes, flashC, reps);
	}

	void setFlashFrame() {
		float t = this->flash_animation.interpolation_a;

		if (t < 0.33f) {
			this->src_color = TazColor::fromVec4(glm::mix(default_src_color.toVec4(), this->flash_animation.flashColor.toVec4(), 3 * t));
		}
		else if (t < 0.66f) {
			this->src_color = TazColor::fromVec4(glm::mix(this->flash_animation.flashColor.toVec4(), default_src_color.toVec4(), std::min((3 * (t - 0.33f)), 1.0f)));
			this->dest_color = TazColor::fromVec4(glm::mix(default_dest_color.toVec4(), this->flash_animation.flashColor.toVec4(), 3 * (t - 0.33f)));
		}
		else {
			this->dest_color = TazColor::fromVec4(glm::mix(this->flash_animation.flashColor.toVec4(), default_dest_color.toVec4(), std::min((3 * (t - 0.66f)), 1.0f)));
		}
		// Smooth transition using lerp (linear interpolation)

	}

	std::string GetComponentName() override {
		return "Line_w_Color";
	}

	void showGUI(std::vector<BaseComponent*> otherComponents = {}) override {
		ImGui::Separator();

		ImVec4 a_color = ImVec4(src_color.r / 255.0f, src_color.g / 255.0f, src_color.b / 255.0f, src_color.a / 255.0f);
		if (ImGui::ColorPicker4("TazColor Line Src", (float*)&a_color)) {
			TazColor newColor = {
					   (GLubyte)(a_color.x * 255),
					   (GLubyte)(a_color.y * 255),
					   (GLubyte)(a_color.z * 255),
					   (GLubyte)(a_color.w * 255)
			};

			setSrcColor(newColor);
		}


		ImVec4 b_color = ImVec4(dest_color.r / 255.0f, dest_color.g / 255.0f, dest_color.b / 255.0f, dest_color.a / 255.0f);
		if (ImGui::ColorPicker4("TazColor Line Dest", (float*)&b_color)) {
			TazColor newColor = {
					   (GLubyte)(b_color.x * 255),
					   (GLubyte)(b_color.y * 255),
					   (GLubyte)(b_color.z * 255),
					   (GLubyte)(b_color.w * 255)
			};

			setDestColor(newColor);
		}
	}
};