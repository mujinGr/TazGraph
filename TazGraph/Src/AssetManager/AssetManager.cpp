#include "AssetManager.h"
#include "GECS/Components.h"
#include "GECS/UtilComponents.h"
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

void AssetManager::CreateWorldMap(Entity& worldMap)
{
	worldMap.addComponent<TransformComponent>(glm::vec2(-TextureManager::getInstance().Get_GLTexture("worldMap")->width / 2, -TextureManager::getInstance().Get_GLTexture("worldMap")->height / 2), Manager::groupBackgroundLayer,
		glm::ivec2(
			TextureManager::getInstance().Get_GLTexture("worldMap")->width,
			TextureManager::getInstance().Get_GLTexture("worldMap")->height
		),
		1.0f);
	worldMap.addComponent<SpriteComponent>("worldMap", false);
	worldMap.addComponent<TransformComponent>(glm::vec2(200.0f, 320.0f), Manager::groupBackgroundLayer, glm::ivec2(4, 4), 1);
	worldMap.addComponent<SpriteComponent>();
	worldMap.addGroup(Manager::panelBackground);
}

void AssetManager::CreateGroup(Entity& groupNode, glm::vec2 centerGroup, float groupNodeSize, Grid::Level m_level)
{
	if (m_level == Grid::Level::Outer1)
	{
		groupNode.addComponent<TransformComponent>(glm::vec2(centerGroup.x - groupNodeSize / 2,
			centerGroup.y - groupNodeSize / 2), Manager::actionLayer, glm::ivec2(groupNodeSize, groupNodeSize), 1);
		groupNode.addComponent<Rectangle_w_Color>();
		groupNode.GetComponent<Rectangle_w_Color>().color = Color(0, 155, 155, 255);

		groupNode.addGroup( Manager::groupGroupNodes_0 );
	}
	else if (m_level == Grid::Level::Outer2) {
		groupNode.addComponent<TransformComponent>(glm::vec2(centerGroup.x - groupNodeSize / 2,
			centerGroup.y - groupNodeSize / 2), Manager::actionLayer, glm::ivec2(groupNodeSize, groupNodeSize), 1);
		groupNode.addComponent<Rectangle_w_Color>();
		groupNode.GetComponent<Rectangle_w_Color>().color = Color(155, 155, 155, 255);

		groupNode.addGroup(Manager::groupGroupNodes_1);

	}
}

void AssetManager::CreateGroupLink(Entity& groupLink, Grid::Level m_level) {
	if (m_level == Grid::Level::Outer1)
	{
		groupLink.addComponent<Line_w_Color>();
		groupLink.GetComponent<Line_w_Color>().setSrcColor(Color(255, 255, 0, 255));
		groupLink.GetComponent<Line_w_Color>().setDestColor(Color(255, 255, 0, 255));

		groupLink.addGroup(Manager::groupGroupLinks_0);
	}
	else if(m_level == Grid::Level::Outer2){
		groupLink.addComponent<Line_w_Color>();
		groupLink.GetComponent<Line_w_Color>().setSrcColor(Color(255, 0, 0, 255));
		groupLink.GetComponent<Line_w_Color>().setDestColor(Color(0, 255, 0, 255));

		groupLink.addGroup(Manager::groupGroupLinks_1);
	}
	
}

void AssetManager::createGroupLayout(Grid::Level m_level) {

	for (const auto& cell : manager->grid->getCells(m_level)) {
		int totalNodes = 0;

		for (const auto& childCell : cell.children)
		{
			for (const auto& node : childCell->nodes) {
				if (!node->isHidden()) {  // Assuming `isHidden()` is a method that tells if the node is hidden
					totalNodes++;
				}
			}
		}

		if (totalNodes == 0) continue;

		float groupNodeSize = 25 * manager->grid->getLevelCellScale(m_level) - 40 / (totalNodes + 1);

		auto& node = manager->addEntity<Node>();

		glm::vec2 centroid(0);
		for (const auto& childCell : cell.children) {
			for (auto& entity : childCell->nodes) {
				if (!entity->isHidden() && !entity->hasGroup(Manager::cursorGroup)) {
					glm::vec2 node_position = entity->GetComponent<TransformComponent>().getCenterTransform();
					centroid += node_position;

					entity->setParentEntity(&node);
					entity->hide();
				}
			}
		}
		
		centroid /= totalNodes;
		CreateGroup(node, centroid, groupNodeSize, m_level);
		manager->grid->addNode(&node, m_level);

		for (const auto& childCell : cell.children)
		{
			for (auto& entity : childCell->nodes) {
				if (entity->isHidden() && !entity->hasGroup(Manager::cursorGroup)) {
					glm::vec2 relativePosition = entity->GetComponent<TransformComponent>().getPosition() - node.GetComponent<TransformComponent>().getCenterTransform();
					entity->GetComponent<TransformComponent>().setPosition_X(relativePosition.x);
					entity->GetComponent<TransformComponent>().setPosition_Y(relativePosition.y);
				}
			}
		}

		// remove all links
		for (const auto& childCell : cell.children) {
			for (auto& link : childCell->links) {
				link->hide();
			}
		}
		

	}

	auto group_links = (m_level == Grid::Level::Outer1) ?
		manager->getGroup(Manager::groupLinks_0) : manager->getGroup(Manager::groupGroupLinks_0);
	for (const auto& link : group_links) {
		// get the links of the inside nodes
		if (link->isHidden()) {
			auto& groups_link = manager->addEntity<Link>(link->getFromNode()->getParentEntity(), link->getToNode()->getParentEntity());
			CreateGroupLink(groups_link, m_level);
			manager->grid->addLink(&groups_link, m_level);
		}
	}
}

void AssetManager::ungroupLayout(Grid::Level m_level) {
	// first destroy the group nodes
	if (manager->grid->getGridLevel() == Grid::Level::Outer1) {
		for (auto& groupNode : manager->getGroup(Manager::groupGroupNodes_0)) {
			groupNode->destroy();
		}
		for (auto& link : manager->getGroup(Manager::groupGroupLinks_0)) {
			link->destroy();
		}
	}
	else if (manager->grid->getGridLevel() == Grid::Level::Outer2) {
		for (auto& groupNode : manager->getGroup(Manager::groupGroupNodes_1)) {
			groupNode->destroy();
		}
		for (auto& link : manager->getGroup(Manager::groupGroupLinks_1)) {
			link->destroy();
		}
	}
	
	Manager::groupLabels label = (m_level == Grid::Level::Outer1) ? Manager::groupNodes_0 : Manager::groupGroupNodes_0;
	Manager::groupLabels link_label = (m_level == Grid::Level::Outer1) ? Manager::groupLinks_0 : Manager::groupGroupLinks_0;

	// reveal all the hidden nodes
	for (auto& entity : manager->getGroup(label)) {
		if (entity->isHidden() && !entity->hasGroup(Manager::cursorGroup)) {
			// ! update the nodes' position based on the parent position
			TransformComponent* parent_tr = &entity->getParentEntity()->GetComponent<TransformComponent>();

			glm::vec2 absolutePosition = entity->GetComponent<TransformComponent>().getPosition() + parent_tr->getCenterTransform();
			entity->GetComponent<TransformComponent>().setPosition_X(absolutePosition.x);
			entity->GetComponent<TransformComponent>().setPosition_Y(absolutePosition.y);

			entity->setParentEntity(nullptr);
			entity->reveal();
			
		}
	}
	for (auto& link : manager->getGroup(link_label)) {
		link->reveal();
	}
}
