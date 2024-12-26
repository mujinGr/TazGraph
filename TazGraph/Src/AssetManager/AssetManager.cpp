#include "AssetManager.h"
#include "GOS/Components.h"
#include "../GOS/ScriptComponents.h"
#include "../Collision/Collision.h"

#include <random>


AssetManager::AssetManager(Manager* man, InputManager& inputManager, TazGraphEngine::Window& window)
	: manager(man), _inputManager(inputManager), _window(window) // todo make triggers manager
{

}

AssetManager::~AssetManager()
{}

void AssetManager::CreateLink(Entity& linkEntity, Entity& sourceEntity, Entity& destEntity)
{
	linkEntity.addComponent<TransformComponent>(glm::vec2(200.0f, 320.0f), Manager::actionLayer, glm::ivec2(64, 64), 1); // 1448 for near pipe, 200 for start
	linkEntity.addComponent<TransformComponent>(glm::vec2(500.0f, 320.0f), Manager::actionLayer, glm::ivec2(64, 64), 1); // 1448 for near pipe, 200 for start

	linkEntity.addGroup(Manager::groupLinks);
}

void AssetManager::CreateCursor(Entity& cursor)
{
	cursor.addComponent<TransformComponent>(glm::vec2(200.0f, 320.0f), Manager::actionLayer, glm::ivec2(4, 4), 1);
	cursor.addComponent<Rectangle_w_Color>();
	cursor.addComponent<Rectangle_w_Color>().color = Color(0, 0, 0, 255);
	cursor.addGroup(Manager::cursorGroup);
}
