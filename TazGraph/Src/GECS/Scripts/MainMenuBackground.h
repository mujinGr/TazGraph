#pragma once

#include "GECS/Animators/AnimatorComponent.h"
#include <Window/Window.h>

class MainMenuBackground : public Component
{
private:
	TransformComponent* transform = nullptr;
	SpriteComponent* sprite = nullptr;

	float elapsedTime = 0.0f;

	TazGraphEngine::Window* window = nullptr;
public: // it is like it has init that creates Animator Component since it inherits it
	

	MainMenuBackground()
	{

	}

	MainMenuBackground(TazGraphEngine::Window* m_window)
	{
		window = m_window;
	}

	~MainMenuBackground() {

	}

	void init() override {
		std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("mainMenu_main"));

		if (!entity->hasComponent<TransformComponent>()) {
			entity->addComponent<TransformComponent>(
				glm::vec2(
					-window->getScreenWidth() / 2,
					-window->getScreenHeight() / 2
				), Layer::action,
				glm::ivec2(
					window->getScreenWidth(),
					window->getScreenHeight()
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
		float newY = -amplitude + amplitude * sin(frequency * elapsedTime);

		transform->position.z = newY;
	}

	std::string GetComponentName() override {
		return "MainMenuBackground";
	}
};