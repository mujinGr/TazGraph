#pragma once

#include "GOS/Animators/AnimatorComponent.h"


class MainMenuBackground : public Component
{
private:
	TransformComponent* transform;
	SpriteComponent* sprite = nullptr;

	float elapsedTime = 0.0f;
public: // it is like it has init that creates Animator Component since it inherits it
	

	MainMenuBackground()
	{

	}

	~MainMenuBackground() {

	}

	void init() override {
		std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("mainMenu_main"));

		if (!entity->hasComponent<TransformComponent>()) {
			entity->addComponent<TransformComponent>(glm::vec2(main_camera2D->getCameraDimensions().x / 2 - TextureManager::getInstance().Get_GLTexture("graphnetwork")->width / 2, 100.0f), Manager::menubackgroundLayer,
				glm::ivec2(
					TextureManager::getInstance().Get_GLTexture("graphnetwork")->width,
					TextureManager::getInstance().Get_GLTexture("graphnetwork")->height
				),
				1.0f);
		}
		if (!entity->hasComponent<SpriteComponent>()) {
			entity->addComponent<SpriteComponent>("graphnetwork", true);
		}
		transform = &entity->GetComponent<TransformComponent>();
		sprite = &entity->GetComponent<SpriteComponent>();

	}

	void update(float deltaTime) override {

		elapsedTime += deltaTime; // Update the accumulated elapsed time
		float amplitude = 100.0f; // Maximum displacement along the Y axis
		float frequency = 0.002f; // How fast the object moves up and down

		// Calculate the new Y position
		float newY = amplitude * sin(frequency * elapsedTime);

		transform->setZIndex(newY);
	}
};