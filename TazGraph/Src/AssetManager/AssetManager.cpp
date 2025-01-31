#include "AssetManager.h"
#include "GECS/Components.h"
#include "../GECS/ScriptComponents.h"
#include "../Collision/Collision.h"

#include <random>


AssetManager::AssetManager(Manager* man, InputManager& inputManager, TazGraphEngine::Window& window)
	: manager(man), _inputManager(inputManager), _window(window) // todo make triggers manager
{

}

AssetManager::~AssetManager()
{}

void AssetManager::CreateCursor(Entity& cursor)
{
	cursor.addComponent<TransformComponent>(glm::vec2(200.0f, 320.0f), Manager::actionLayer, glm::ivec2(4, 4), 1);
	cursor.addComponent<Rectangle_w_Color>();
	cursor.addComponent<Rectangle_w_Color>().color = Color(0, 0, 0, 255);
	cursor.addGroup(Manager::cursorGroup);
}

void AssetManager::CreateGroup(Entity& groupNode, SDL_FRect centerCell, float groupNodeSize)
{
	groupNode.addComponent<TransformComponent>(glm::vec2(centerCell.x + centerCell.w / 2 - groupNodeSize / 2,
		centerCell.y + centerCell.h / 2 - groupNodeSize / 2), Manager::actionLayer, glm::ivec2(groupNodeSize, groupNodeSize), 1);
	groupNode.addComponent<Rectangle_w_Color>();
	groupNode.GetComponent<Rectangle_w_Color>().color = Color(0, 155, 155, 255);

	groupNode.addGroup(Manager::groupGroupNodes_0);
}

void AssetManager::CreateGroupLink(Entity& groupLink) {
	groupLink.addComponent<Line_w_Color>();
	groupLink.GetComponent<Line_w_Color>().src_color = Color(255, 255, 0, 255);
	groupLink.GetComponent<Line_w_Color>().dest_color = Color(255, 255, 0, 255);

	groupLink.addGroup(Manager::groupGroupLinks_0);
}