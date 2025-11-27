#include "GECSManager.h"

void Manager::updateActiveEntities() {
	std::vector<Entity*> empty_toBeRemoved;
	std::vector<Entity*> nodes_toBeRemoved;
	std::vector<Entity*> links_toBeRemoved;

	for (auto v_entity : visible_emptyEntities)
	{
		if (!v_entity->isActive()) {
			v_entity->removeFromCell();
			empty_toBeRemoved.push_back(v_entity);
		}
	}

	for (auto& v_entity : visible_nodes)
	{
		if (!v_entity->isActive()) {
			v_entity->removeFromCell();
			nodes_toBeRemoved.push_back(v_entity);
		}
	}

	for (auto v_entity : visible_links)
	{
		if (!v_entity->isActive()) {
			v_entity->removeFromCells();
			links_toBeRemoved.push_back(v_entity);
		}
	}
	// if from visible entities is something deleted, then delete it from all data structures (groupedEntities + enentities)
	// ! or instead of updating the groupedEntities when we see an inactive entity, update the groupedEntities the moment an entity goes
	// ! inactive and wait until we about to delete more
	for (auto& group : groupedEmptyEntities) {
		for (auto entity : group) {
			if (!entity->isActive()) {
				entity->removeFromCell();
				empty_toBeRemoved.push_back(entity);
			}
		}
	}
	for (auto& group : groupedNodeEntities) {
		for (auto entity : group) {
			if (!entity->isActive()) {
				entity->removeFromCell();
				nodes_toBeRemoved.push_back(entity);
			}
		}
	}
	for (auto& group : groupedLinkEntities) {
		for (auto entity : group) {
			if (!entity->isActive()) {
				entity->removeFromCells();
				links_toBeRemoved.push_back(entity);
			}
		}
	}
	{
		std::shared_lock lock(entities_mtx);

		for (auto i(0u); i < maxGroups; i++) {
			auto& group(visible_groupedEmptyEntities[i]);
			group.erase(std::remove_if(std::begin(group), std::end(group),
				[this, &empty_toBeRemoved, i](Entity* mEntity) {
					return !mEntity->isActive()
						|| !mEntity->hasGroup(i);
				}), group.end());
			auto& m_group(groupedEmptyEntities[i]);
			m_group.erase(std::remove_if(std::begin(m_group), std::end(m_group),
				[this, &empty_toBeRemoved, i](Entity* mEntity) {
					return !mEntity->isActive()
						|| !mEntity->hasGroup(i);
				}), m_group.end());
		}

		for (auto i(0u); i < maxGroups; i++) {
			auto& group(visible_groupedNodeEntities[i]);
			group.erase(std::remove_if(std::begin(group), std::end(group),
				[this, &nodes_toBeRemoved, i](Entity* mEntity) {
					return !mEntity->isActive()
						|| !mEntity->hasGroup(i);
				}), group.end());
			auto& m_group(groupedNodeEntities[i]);
			m_group.erase(std::remove_if(std::begin(m_group), std::end(m_group),
				[this, &nodes_toBeRemoved, i](Entity* mEntity) {
					return !mEntity->isActive()
						|| !mEntity->hasGroup(i);
				}), m_group.end());
		}

		for (auto i(0u); i < maxGroups; i++) {
			auto& group(visible_groupedLinkEntities[i]);
			group.erase(std::remove_if(std::begin(group), std::end(group),
				[this, &links_toBeRemoved, i](Entity* mEntity) {
					return !mEntity->isActive()
						|| !mEntity->hasGroup(i);
				}), group.end());
			auto& m_group(groupedLinkEntities[i]);
			m_group.erase(std::remove_if(std::begin(m_group), std::end(m_group),
				[this, &links_toBeRemoved, i](Entity* mEntity) {
					return !mEntity->isActive()
						|| !mEntity->hasGroup(i);
				}), m_group.end());
		}
	}



	visible_emptyEntities.erase(std::remove_if(visible_emptyEntities.begin(), visible_emptyEntities.end(),
		[&empty_toBeRemoved](Entity* mEntity) {
			return std::find(empty_toBeRemoved.begin(), empty_toBeRemoved.end(), mEntity) != empty_toBeRemoved.end();
		}),
		visible_emptyEntities.end());
	visible_nodes.erase(std::remove_if(visible_nodes.begin(), visible_nodes.end(),
		[&nodes_toBeRemoved](Entity* mEntity) {
			return std::find(nodes_toBeRemoved.begin(), nodes_toBeRemoved.end(), mEntity) != nodes_toBeRemoved.end();
		}),
		visible_nodes.end());

	visible_links.erase(std::remove_if(visible_links.begin(), visible_links.end(),
		[&links_toBeRemoved](Entity* mEntity) {
			return std::find(links_toBeRemoved.begin(), links_toBeRemoved.end(), mEntity) != links_toBeRemoved.end();
		}),
		visible_links.end());

	std::vector<Entity*> toRemove;
	toRemove.reserve(
		empty_toBeRemoved.size() + nodes_toBeRemoved.size() + links_toBeRemoved.size()
	);

	// collect all ids
	for (auto e : empty_toBeRemoved)
		toRemove.push_back(e);
	for (auto e : nodes_toBeRemoved)
		toRemove.push_back(e);
	for (auto e : links_toBeRemoved)
		toRemove.push_back(e);

	// remove by id
	{
		std::unique_lock lock(entities_mtx);
		for (Entity* e : toRemove)
			entities.erase(e->getId());
	}
}

void Manager::updateVisibleEntities() {
	visible_emptyEntities = getRevealedEntitiesInCameraCells<EmptyEntity>();
	visible_nodes = getRevealedEntitiesInCameraCells<NodeEntity>();
	visible_links = getRevealedEntitiesInCameraCells<LinkEntity>();

	for (auto& vgroup : visible_groupedEmptyEntities) {
		vgroup.clear();
	}
	for (auto& vgroup : visible_groupedNodeEntities) {
		vgroup.clear();
	}
	for (auto& vgroup : visible_groupedLinkEntities) {
		vgroup.clear();
	}

	for (auto ventity : visible_emptyEntities) {
		if (!ventity->isActive()) {
			continue;
		}

		for (unsigned i = 0; i < maxGroups; ++i) {
			if (ventity->hasGroup(i)) {
				visible_groupedEmptyEntities[i].push_back(ventity);
			}
		}
	}
	for (auto ventity : visible_nodes) {
		if (!ventity->isActive()) {
			continue;
		}

		for (unsigned i = 0; i < maxGroups; ++i) {
			if (ventity->hasGroup(i)) {
				visible_groupedNodeEntities[i].push_back(ventity);
			}
		}
	}
	for (auto ventity : visible_links) {
		if (!ventity->isActive()) {
			continue;
		}

		for (unsigned i = 0; i < maxGroups; ++i) {
			if (ventity->hasGroup(i)) {
				visible_groupedLinkEntities[i].push_back(ventity);
			}
		}
	}
}


std::string Manager::getGroupName(Group mGroup) const {
	return groupNames.at(mGroup);
}

void Manager::scanComponentNames(const std::string& folderPath) {
	std::regex classRegex(R"class(\s*class\s+([A-Za-z0-9_]+)\s*:\s*public\s+([A-Za-z0-9_]+))class");

	for (const auto& entry : fs::recursive_directory_iterator(folderPath)) {
		if (entry.path().extension() == ".h" || entry.path().extension() == ".hpp") {
			std::ifstream file(entry.path());
			std::string line;
			while (std::getline(file, line)) {
				std::smatch match;
				if (std::regex_search(line, match, classRegex)) {
					std::string className = match[1].str();
					std::string baseClass = match[2].str();

					if (baseClass == "Component" || baseClass == "AnimatorComponent") {
						componentNames["Component"].push_back(className);
					}
					else if (baseClass == "NodeComponent") {
						componentNames["NodeComponent"].push_back(className);
					}
					else if (baseClass == "LinkComponent") {
						componentNames["LinkComponent"].push_back(className);
					}
				}
			}
		}
	}
}

void Manager::setComponentNames()
{

	std::cout << "Current Working Directory: " << fs::current_path() << std::endl;

	const std::string folderPath = fs::current_path().string() + "/../TazGraphEngine/src/GECS";
	if (!fs::exists(folderPath)) {
		std::cerr << "Error: Folder does not exist at " << folderPath << std::endl;
	}

	scanComponentNames(folderPath);

}

std::vector<Entity*> Manager::collectEntities(
	std::initializer_list<Manager::groupLabels> groupNames,
	Taz::EntityType type
)
{
	std::vector<Entity*> result;

	if (type == Taz::EntityType::Empty) {
		for (const auto& groupName : groupNames) {
			auto& group = getVisibleGroup<EmptyEntity>(groupName);
			result.insert(result.end(), group.begin(), group.end());
		}
	}
	else if (type == Taz::EntityType::Node) {
		for (const auto& groupName : groupNames) {
			auto& group = getVisibleGroup<NodeEntity>(groupName);
			result.insert(result.end(), group.begin(), group.end());
		}
	}
	else if (type == Taz::EntityType::Link) {
		for (const auto& groupName : groupNames) {
			auto& group = getVisibleGroup<LinkEntity>(groupName);
			result.insert(result.end(), group.begin(), group.end());
		}
	}
	else if (type == Taz::EntityType::Minimap) {
		for (const auto& groupName : groupNames) {
			auto& group = getGroup<NodeEntity>(groupName);
			result.insert(result.end(), group.begin(), group.end());
		}
	}

	return result;
}


;

// loops through the intrecepted cells and just get the entities
template <typename T>
std::vector<Entity*> Manager::getRevealedEntitiesInCameraCells() {
	std::vector<Entity*> result;

	if constexpr (std::is_same_v<T, NodeEntity>) {
		for (auto& cell : grid->interceptedCells) {
			for (auto& entityId : cell->nodes) {
				auto* ent = getEntityFromId(entityId);

				if (!ent->isHidden()) {  // Check if the entity is visible
					result.push_back(ent);

					for (auto& port : ent->children) {
						if (hasEntity(port.second)) {
							Entity* childEnt = getEntityFromId(port.second);
							if (!childEnt->isHidden()) {
								visible_emptyEntities.push_back(childEnt);

								if (childEnt->hasComponent<PortComponent>()) {
									for (auto& portSlots : childEnt->children)
										visible_emptyEntities.push_back(getEntityFromId(portSlots.second));
								}
							}
						}
					}
				}
			}
		}
	}
	else if constexpr (std::is_same_v<T, EmptyEntity>) {
		for (auto& cell : grid->interceptedCells) {
			for (auto entityId : cell->emptyEntities) {
				auto* entity = getEntityFromId(entityId);

				if (!entity->isHidden()) {  // Check if the entity is visible
					result.push_back(entity);
				}
			}
		}
	}
	else if constexpr (std::is_same_v<T, LinkEntity>) {
		std::unordered_set<EntityID> uniqueIds;

		for (auto& cell : grid->interceptedCells) {
			for (auto& linkId : cell->links) {

				auto* link = getEntityFromId(linkId);

				if (!link->isHidden()) {
					if (uniqueIds.find(linkId) == uniqueIds.end()) {
						uniqueIds.insert(linkId);
					}
				}
			}
		}
		for (auto& entry : uniqueIds) {
			Entity* e = getEntityFromId(entry); // or entities[id]
			if (e)
				result.push_back(e);
		}
	}
	else {
		static_assert(sizeof(T) == 0, "Unsupported entity type.");
	}
	return result;

}
