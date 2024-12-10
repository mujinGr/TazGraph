#include "AssetManager.h"
#include "ECS/Components.h"
#include "../ECS/ScriptComponents.h"
#include "../Collision/Collision.h"

#include <random>


AssetManager::AssetManager(Manager* man, InputManager& inputManager, TazGraphEngine::Window& window)
	: manager(man), _inputManager(inputManager), _window(window) // todo make triggers manager
{

}

AssetManager::~AssetManager()
{}

void AssetManager::CreatePlayer(Entity& player)
{
	player.addComponent<TransformComponent>(glm::vec2(200.0f, 320.0f), Manager::actionLayer, glm::ivec2(64, 64), 1); // 1448 for near pipe, 200 for start
	player.addComponent<KeyboardControllerComponent>(
		_inputManager,
		SDLK_w,
		SDLK_a,
		SDLK_d,
		SDLK_s
		);
	player.addComponent<ScoreComponent>();

	player.addGroup(Manager::groupPlayers);
}
