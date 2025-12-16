#pragma once

#include "../../../Components.h"
#include "../../../Animators/MovingAnimation.h"

class TransformComponent : public Component //transform as in graphics, we have rotation and scale
{
public:
	glm::vec3 velocity = glm::vec3(0);
	glm::vec3 rotation = { 0.0f,0.0f,0.0f };
	glm::vec3 position = glm::vec3(0);
	glm::vec3 local_position = glm::vec3(0);
	glm::vec3 local_normal_position = glm::vec3(0);
	glm::vec3 size = glm::vec3(0);

	glm::vec3 last_position = glm::vec3(0);
	glm::vec3 last_size = glm::vec3(0);
	glm::vec3 last_velocity = glm::vec3(0);

	float scale = 1;

	int speed = 1;
	MovingAnimation moving_animation;


	TransformComponent()
	{
	}

	TransformComponent(float sc)
	{
		scale = sc;
	}

	TransformComponent(glm::vec2 m_position)
	{
		position.x = m_position.x;
		position.y = m_position.y;
	}

	TransformComponent(glm::vec3 m_position)
	{
		position = m_position;
	}

	TransformComponent(glm::vec2 m_position, layer layer, glm::vec2 m_size, float sc) : TransformComponent(m_position) {
		position = { m_position.x, m_position.y, getLayerDepth(layer) };
		size = { m_size.x, m_size.y, 0.0f };
		scale = sc;
	}

	TransformComponent(glm::vec2 m_position, layer layer, glm::vec2 size, float sc, int sp) : TransformComponent(m_position, layer, size, sc)
	{
		speed = sp;
	}

	TransformComponent(glm::vec2 m_position, layer layer, glm::vec3 m_size, float sc) : TransformComponent(m_position) {
		size = m_size;
		scale = sc;
	}

	TransformComponent(glm::vec2 m_position, layer layer, glm::vec3 size, float sc, int sp) : TransformComponent(m_position, layer, size, sc)
	{
		speed = sp;
	}

	TransformComponent(glm::vec3 m_position, glm::vec3 m_size, float sc) : TransformComponent(m_position) {
		size = m_size;
		scale = sc;
	}

	void init() override
	{
	}
	void update(float deltaTime) override
	{

		if (entity->getParentEntity()
			&& (dynamic_cast<NodeEntity*>(entity->getParentEntity())
				|| dynamic_cast<EmptyEntity*>(entity->getParentEntity())))
		{
			Entity* parent = entity->getParentEntity();
			TransformComponent* parentTR = &parent->GetComponent<TransformComponent>();
			if (!glm::all(glm::equal(local_normal_position, glm::vec3(0.0f)))) {
				position = parentTR->getPosition() + local_normal_position * parentTR->size / 2.0f;
			}
			else {
				position = parentTR->getPosition() + local_position;
			}

		}

		if (position == last_position && size == last_size && velocity == last_velocity) {
			return;
		}

		entity->cellUpdate();

		last_position = position;
		last_size = size;
		last_velocity = velocity;

		position.x += velocity.x * speed * deltaTime;
		position.y += velocity.y * speed * deltaTime;

		velocity *= 0.98f;

	}

	void initChild() {
		if (entity->getParentEntity()
			&& (dynamic_cast<NodeEntity*>(entity->getParentEntity())
				|| dynamic_cast<EmptyEntity*>(entity->getParentEntity())))
		{
			Entity* parent = entity->getParentEntity();
			TransformComponent* parentTR = &parent->GetComponent<TransformComponent>();

			if (!glm::all(glm::equal(local_normal_position, glm::vec3(0.0f)))) {
				position = parentTR->getPosition() + local_normal_position * parentTR->size / 2.0f;
			}
			else {
				position = parentTR->getPosition() + local_position;
			}

		}
	}

	glm::vec3 getSizeCenter() {
		return glm::vec3(size.x * scale / 2, size.y * scale / 2, size.z * scale / 2);
	}

	glm::vec3 getPosition() {
		return position;
	}

	void setPosition_X(float newPosition_X) {
		position.x = newPosition_X;
	}
	void setPosition_Y(float newPosition_Y) {
		position.y = newPosition_Y;
	}

	glm::vec3 getVelocity() {
		return velocity;
	}

	void setVelocity_X(float newVelocity_X) {
		velocity.x = newVelocity_X;
	}
	void setVelocity_Y(float newVelocity_Y) {
		velocity.y = newVelocity_Y;
	}

	void setRotation(glm::vec3 m_rotation) {
		rotation = m_rotation;
	}

	void SetMovingAnimation(
		glm::vec3 m_startingPos,
		size_t fr,
		float sp,
		const Animation::animType type,
		const glm::vec3 distance,
		const glm::vec3 dest_rotation,
		int reps = 0)
	{
		moving_animation = MovingAnimation(m_startingPos, fr, sp, type, distance, dest_rotation, reps);
	}

	void setMoveFrame() {

		float progress = (float)moving_animation.cur_frame_index /
			(float)moving_animation.total_frames;

		// Use linear interpolation
		float newX = moving_animation.startingPosition.x +
			((moving_animation.distance.x) * progress);

		float newY = moving_animation.startingPosition.y +
			((moving_animation.distance.y) * progress); // Fixed typo: was .x instead of .y

		setPosition_X(newX);
		setPosition_Y(newY);
	}

	std::string GetComponentName() override {
		return "TransformComponent";
	}

	void showGUI(std::vector<BaseComponent*> otherComponents = {}) override {
		ImGui::Separator();

		// TazPosition Controls
		ImGui::Text("TazPosition:");
		ImGui::SliderFloat3("##position", &position.x, -1000.0f, 1000.0f);

		if (ImGui::Button("Apply TazPosition to All##transform_pos")) {
			modifyPosition = true;
		}
		if (modifyPosition && !otherComponents.empty()) {
			for (auto& comp : otherComponents) {
				if (auto* other = dynamic_cast<TransformComponent*>(comp)) {
					other->position = position;
				}
			}
		}

		// TazSize Controls
		ImGui::Text("TazSize:");
		ImGui::SliderFloat3("##size", &size.x, 1.0f, 100.0f);

		// TazRotation Controls
		ImGui::Text("TazRotation:");
		ImGui::SliderFloat3("##rotation", glm::value_ptr(rotation), -180.0f, 180.0f);

		ImGui::Text("Scale:");
		ImGui::SliderFloat("##scale", &scale, 0.1f, 10.0f);

		// Speed Control
		ImGui::Text("Speed:");
		ImGui::InputInt("##speed", &speed);
	};
};