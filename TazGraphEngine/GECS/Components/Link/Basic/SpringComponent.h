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
		NodeEntity* a = entity->getFromNode();
		NodeEntity* b = entity->getToNode();

		glm::vec3 posA = a->GetComponent<TransformComponent>().bodyCenter;
		glm::vec3 posB = b->GetComponent<TransformComponent>().bodyCenter;

		glm::vec3 delta = posB - posA;
		glm::vec3 attraction = delta * springStrength;

		if (glm::length(delta) > deltaThreshold) {
			a->GetComponent<TransformComponent>().velocity += attraction;
			b->GetComponent<TransformComponent>().velocity -= attraction;
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

	void showGUI() override {
		ImGui::Separator();

		ImGui::Text("SpringComponent");

	}
};