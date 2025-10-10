#include "AssetManager.h"

#include <random>
#include <unordered_set>

void AssetManager::CreateWorldMap(Entity& worldMap)
{
	worldMap.addComponent<TransformComponent>(glm::vec2(-TextureManager::getInstance().Get_GLTexture("worldMap")->width / 2, -TextureManager::getInstance().Get_GLTexture("worldMap")->height / 2), Manager::groupBackgroundLayer,
		glm::ivec2(
			TextureManager::getInstance().Get_GLTexture("worldMap")->width,
			TextureManager::getInstance().Get_GLTexture("worldMap")->height
		),
		1.0f);
	worldMap.addComponent<SpriteComponent>("worldMap");
	worldMap.addComponent<TransformComponent>(glm::vec2(200.0f, 320.0f), Manager::groupBackgroundLayer, glm::ivec2(4, 4), 1);
	worldMap.addComponent<SpriteComponent>();
	worldMap.addGroup(Manager::panelBackground);
}

void AssetManager::CreateGroup(Entity& groupNode, glm::vec3 centerGroup, float groupNodeSize, Grid::Level m_level)
{
	if (m_level == Grid::Level::Outer1)
	{
		groupNode.addComponent<TransformComponent>(glm::vec3(centerGroup.x,
			centerGroup.y, centerGroup.z), Layer::action, glm::vec3(groupNodeSize), 1);
		groupNode.addComponent<Rectangle_w_Color>();
		groupNode.GetComponent<Rectangle_w_Color>().color = TazColor(0, 155, 155, 255);

		groupNode.addGroup(Manager::groupGroupNodes_0);
	}
	else if (m_level == Grid::Level::Outer2) {
		groupNode.addComponent<TransformComponent>(glm::vec2(centerGroup.x,
			centerGroup.y), Layer::action, glm::vec3(groupNodeSize), 1);
		groupNode.addComponent<Rectangle_w_Color>();
		groupNode.GetComponent<Rectangle_w_Color>().color = TazColor(155, 155, 155, 255);

		groupNode.addGroup(Manager::groupGroupNodes_1);

	}
}

void AssetManager::CreateGroupLink(Entity& groupLink, Grid::Level m_level) {
	if (m_level == Grid::Level::Outer1)
	{
		groupLink.addComponent<Line_w_Color>();
		groupLink.GetComponent<Line_w_Color>().setSrcColor(TazColor(255, 255, 0, 255));
		groupLink.GetComponent<Line_w_Color>().setDestColor(TazColor(255, 0, 0, 255));

		groupLink.addGroup(Manager::groupGroupLinks_0);
	}
	else if (m_level == Grid::Level::Outer2) {
		groupLink.addComponent<Line_w_Color>();
		groupLink.GetComponent<Line_w_Color>().setSrcColor(TazColor(255, 0, 0, 255));
		groupLink.GetComponent<Line_w_Color>().setDestColor(TazColor(0, 255, 0, 255));

		groupLink.addGroup(Manager::groupGroupLinks_1);
	}

}

void AssetManager::createGroupLayout(Manager* manager, Grid::Level m_level) {

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

		float groupNodeSize = 25.0f * manager->grid->getLevelCellScale(m_level) - 40.0f / (totalNodes + 1.0f);

		auto& node = manager->addEntity<Node>();

		glm::vec3 centroid(0);
		for (const auto& childCell : cell.children) {
			for (auto& entity : childCell->nodes) {
				if (!entity->isHidden()) {
					glm::vec3 node_position = entity->GetComponent<TransformComponent>().getPosition();
					centroid += node_position;

					entity->setParentEntity(&node);
					entity->hide();
				}
			}
		}

		centroid /= totalNodes;
		AssetManager::CreateGroup(node, centroid, groupNodeSize, m_level);

		if (manager->arrowheadsEnabled) {
			manager->arrowheadsEnabled = false;
		}

		manager->grid->addNode(&node, m_level);

		for (const auto& childCell : cell.children)
		{
			for (auto& entity : childCell->nodes) {
				if (entity->isHidden()) {
					glm::vec2 relativePosition =
						entity->GetComponent<TransformComponent>().getPosition() -
						node.GetComponent<TransformComponent>().getPosition();

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

	auto child_links = (m_level == Grid::Level::Outer1) ?
		manager->getGroup<LinkEntity>(Manager::groupLinks_0) : manager->getGroup<LinkEntity>(Manager::groupGroupLinks_0);

	std::unordered_set<std::pair<EntityID, EntityID>, PairHash> existingLinks;

	for (const auto& c_link : child_links) {
		if (c_link->isHidden()) {
			// Get the parent group nodes of the source and target
			auto* fromNode = c_link->getFromNode();
			auto* toNode = c_link->getToNode();
			auto* fromParent = fromNode->getParentEntity();
			auto* toParent = toNode->getParentEntity();

			// Skip if:
			// - Either parent is missing (not grouped).
			// - Source and target are in the same group (no self-links).
			if (!fromParent || !toParent || fromParent == toParent) {
				continue;
			}

			// Create a unique key for the link (sorted to avoid duplicates like A-B vs B-A)
			EntityID parentFromId = fromParent->getId();
			EntityID parentToId = toParent->getId();
			auto linkKey = (parentFromId < parentToId) ?
				std::make_pair(parentFromId, parentToId) :
				std::make_pair(parentToId, parentFromId);

			// Add the link if it doesn't already exist
			if (existingLinks.insert(linkKey).second) {
				auto& group_link = manager->addEntity<Link>(parentFromId, parentToId);
				AssetManager::CreateGroupLink(group_link, m_level);  // Customize appearance if needed
				manager->grid->addLink(&group_link, m_level);
			}
		}
	}
}

void AssetManager::ungroupLayout(Manager* manager, Grid::Level m_level) {
	// first destroy the group nodes
	if (manager->grid->getGridLevel() == Grid::Level::Outer1) {
		for (auto& groupNode : manager->getGroup<NodeEntity>(Manager::groupGroupNodes_0)) {
			groupNode->destroy();
		}
		for (auto& link : manager->getGroup<LinkEntity>(Manager::groupGroupLinks_0)) {
			link->destroy();
		}
	}
	else if (manager->grid->getGridLevel() == Grid::Level::Outer2) {
		for (auto& groupNode : manager->getGroup<NodeEntity>(Manager::groupGroupNodes_1)) {
			groupNode->destroy();
		}
		for (auto& link : manager->getGroup<LinkEntity>(Manager::groupGroupLinks_1)) {
			link->destroy();
		}
	}

	Manager::groupLabels label = (m_level == Grid::Level::Outer1) ? Manager::groupNodes_0 : Manager::groupGroupNodes_0;
	Manager::groupLabels link_label = (m_level == Grid::Level::Outer1) ? Manager::groupLinks_0 : Manager::groupGroupLinks_0;

	if (manager->arrowheadsEnabled) {
		manager->arrowheadsEnabled = false;
	}

	// reveal all the hidden nodes
	for (auto& entity : manager->getGroup<NodeEntity>(label)) {
		if (entity->isHidden()) {
			// ! update the nodes' position based on the parent position
			TransformComponent* parent_tr = &entity->getParentEntity()->GetComponent<TransformComponent>();

			glm::vec2 absolutePosition = entity->GetComponent<TransformComponent>().getPosition() + parent_tr->getPosition();
			entity->GetComponent<TransformComponent>().setPosition_X(absolutePosition.x);
			entity->GetComponent<TransformComponent>().setPosition_Y(absolutePosition.y);

			entity->setParentEntity(nullptr);
			entity->reveal();

		}
	}
	for (auto& link : manager->getGroup<LinkEntity>(link_label)) {
		link->reveal();
	}
}

void AssetManager::AddDefaultNode(Entity& node, glm::vec3 mPosition)
{
	static int colorOffset = 0;
	colorOffset = (colorOffset + 2) % 256; // Vary color slightly each time

	//create Node function
	node.addComponent<TransformComponent>(mPosition, glm::vec3(10.0f), 1);
	node.addComponent<Rectangle_w_Color>();
	node.GetComponent<Rectangle_w_Color>().setColor(TazColor(0, colorOffset, 224, 255));

	node.addComponent<RectangleFlashAnimatorComponent>();
}

void AssetManager::AddTreeNode(Entity& node, glm::vec3 mPosition)
{
	static int colorOffset = 0;
	colorOffset = (colorOffset + 2) % 256; // Vary color slightly each time

	//create Node function
	node.addComponent<TransformComponent>(mPosition, glm::vec3(10.0f), 1);
	node.addComponent<Rectangle_w_Color>();
	node.GetComponent<Rectangle_w_Color>().setColor(TazColor(0, colorOffset, 224, 255));

	node.addComponent<RectangleFlashAnimatorComponent>();
	node.addComponent<PollingComponent>();
}

void AssetManager::AddDefaultLink(Entity& link)
{
	link.addComponent<Line_w_Color>();

	link.GetComponent<Line_w_Color>().setSrcColor(TazColor(255, 40, 0, 255));
	link.GetComponent<Line_w_Color>().setDestColor(TazColor(40, 255, 0, 255));

	link.addComponent<LineFlashAnimatorComponent>();
}

void AssetManager::AddTreeLink(Entity& link)
{
	link.addComponent<Line_w_Color>();

	link.GetComponent<Line_w_Color>().setSrcColor(TazColor(40, 255, 0, 255));
	link.GetComponent<Line_w_Color>().setDestColor(TazColor(40, 255, 0, 255));

	link.addComponent<LineFlashAnimatorComponent>();
}

void AssetManager::AddPathLink(Entity& link)
{
	link.addComponent<Line_w_Color>();

	link.GetComponent<Line_w_Color>().setSrcColor(TazColor(0, 0, 255, 255));
	link.GetComponent<Line_w_Color>().setDestColor(TazColor(40, 0, 255, 255));
}

void AssetManager::AddSimulationNode(Entity& node, glm::vec3 pos = glm::vec3(0))
{
	node.addComponent<RectangleFlashAnimatorComponent>();

	node.addComponent<MovingAnimatorComponent>();
}

void AssetManager::AddSimulationLink(Entity& link)
{
	link.addComponent<LineFlashAnimatorComponent>();
}
