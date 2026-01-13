#pragma once

#include "../../../Components.h"

class SpringComponent : public LinkComponent
{
private:
	int deltaThreshold = 300;
	float springStrength = 0.00002f;
public:

	SpringComponent()
	{

	}

	~SpringComponent() {

	}

	void init() override {
	}

	void update(float deltaTime) override {

		glm::vec3 posA = entity->fromPos;
		glm::vec3 posB = entity->toPos;

		glm::vec3 delta = posB - posA;
		glm::vec3 attraction = delta * springStrength;

		if (glm::length(delta) > deltaThreshold) {
			entity->getManager()->getEntityFromId(
				entity->getFromNode()
			)->GetComponent<TransformComponent>().velocity += attraction;
			entity->getManager()->getEntityFromId(
				entity->getToNode()
			)->GetComponent<TransformComponent>().velocity -= attraction;
		}

		/*springStrength -= 0.001f;
		if (springStrength < 0) {
			entity->removeComponent<SpringComponent>();
		}*/
	}

	void draw(size_t v_index, LineRenderer& batch, TazGraphEngine::Window& window) {
	}

	std::string GetComponentName() override {
		return "SpringComponent";
	}

	void showGUI(std::vector<BaseComponent*> otherComponents = {}) override {
		ImGui::Separator();

		ImGui::Text("SpringComponent");

	}

	void showGUI(std::vector<BaseComponent*> otherComponents, std::vector<Entity*> otherEntities) override {
		ImGui::Separator();

		ImGui::Text("SpringComponent");
	};
};