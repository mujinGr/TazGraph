#include "AssetManager.h"
#include "ECS/Components.h"
#include "../ECS/ScriptComponents.h"
#include "../Collision/Collision.h"

#include <random>


AssetManager::AssetManager(Manager* man, InputManager& inputManager, MujinEngine::Window& window)
	: manager(man), _inputManager(inputManager), _window(window) // todo make triggers manager
{
	onpipeTriggers[0].x = 1840;
	onpipeTriggers[0].y = 512;
	onpipeTriggers[0].w = 24;
	onpipeTriggers[0].h = 8;

	onpipeTriggers[1].x = 2608;
	onpipeTriggers[1].y = 448;
	onpipeTriggers[1].w = 32;
	onpipeTriggers[1].h = 8;

	leftofpipeTriggers[0].x = 3752;
	leftofpipeTriggers[0].y = 608; 
	leftofpipeTriggers[0].w = 16; 
	leftofpipeTriggers[0].h = 8; 
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

bool AssetManager::OnPipeTrigger(SDL_Rect collider) const
{
	for (int i = 0; i < 2; i++)
	{
		if (Collision::checkCollision(onpipeTriggers[i], collider)) {
			return true;
		}
	}
	return false;
}

bool AssetManager::LeftOfPipeTrigger(SDL_Rect collider) const
{
	for (int i = 0; i < 1; i++)
	{
		if (Collision::checkCollision(leftofpipeTriggers[i], collider)) {
			return true;
		}
	}
	return false;
}