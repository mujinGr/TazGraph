#include "GECSManager.h"

void Manager::updateActiveEntities() {
	std::vector<EntityID> toBeRemoved;
	std::vector<EntityID> nodes_toBeRemoved;
	std::vector<EntityID> links_toBeRemoved;

	for (auto v_entityId : visible_emptyEntities)
	{
		auto* ent = getEntityFromId(v_entityId);
		if (ent->isActive()) {
			ent->removeFromCell();
			toBeRemoved.push_back(v_entityId);
		}
	}

	for (auto& v_entityId : visible_nodes)
	{
		auto* ent = getEntityFromId(v_entityId);
		if (!ent->isActive()) {
			ent->removeFromCell();
			nodes_toBeRemoved.push_back(v_entityId);
		}
	}

	for (auto v_entityId : visible_links)
	{
		auto* ent = getEntityFromId(v_entityId);
		if (!ent->isActive()) {
			ent->removeFromCells();
			links_toBeRemoved.push_back(v_entityId);
		}
	}
	// if from visible entities is something deleted, then delete it from all data structures (groupedEntities + enentities)
	// ! or instead of updating the groupedEntities when we see an inactive entity, update the groupedEntities the moment an entity goes
	// ! inactive and wait until we about to delete more
	for (auto& group : groupedEmptyEntities) {
		for (auto entityId : group) {
			auto* ent = getEntityFromId(entityId);
			if (!ent->isActive()) {
				ent->removeFromCell();
				toBeRemoved.push_back(entityId);
			}
		}
	}
	for (auto& group : groupedNodeEntities) {
		for (auto entityId : group) {
			auto* ent = getEntityFromId(entityId);
			if (!ent->isActive()) {
				ent->removeFromCell();
				nodes_toBeRemoved.push_back(entityId);
			}
		}
	}
	for (auto& group : groupedLinkEntities) {
		for (auto entityId : group) {
			auto* ent = getEntityFromId(entityId);
			if (!ent->isActive()) {
				ent->removeFromCells();
				links_toBeRemoved.push_back(entityId);
			}
		}
	}
	{
		std::shared_lock lock(entities_mtx);

		for (auto i(0u); i < maxGroups; i++) {
			auto& group(visible_groupedEmptyEntities[i]);
			group.erase(std::remove_if(std::begin(group), std::end(group),
				[this, &toBeRemoved, i](EntityID mEntity) {
					return !entities[mEntity]->isActive()
						|| !entities[mEntity]->hasGroup(i);
				}), group.end());
			auto& m_group(groupedEmptyEntities[i]);
			m_group.erase(std::remove_if(std::begin(m_group), std::end(m_group),
				[this, &toBeRemoved, i](EntityID mEntity) {
					return !entities[mEntity]->isActive()
						|| !entities[mEntity]->hasGroup(i);
				}), m_group.end());
		}

		for (auto i(0u); i < maxGroups; i++) {
			auto& group(visible_groupedNodeEntities[i]);
			group.erase(std::remove_if(std::begin(group), std::end(group),
				[this, &nodes_toBeRemoved, i](EntityID mEntity) {
					return !entities[mEntity]->isActive()
						|| !entities[mEntity]->hasGroup(i);
				}), group.end());
			auto& m_group(groupedNodeEntities[i]);
			m_group.erase(std::remove_if(std::begin(m_group), std::end(m_group),
				[this, &nodes_toBeRemoved, i](EntityID mEntity) {
					return !entities[mEntity]->isActive()
						|| !entities[mEntity]->hasGroup(i);
				}), m_group.end());
		}

		for (auto i(0u); i < maxGroups; i++) {
			auto& group(visible_groupedLinkEntities[i]);
			group.erase(std::remove_if(std::begin(group), std::end(group),
				[this, &links_toBeRemoved, i](EntityID mEntity) {
					return !entities[mEntity]->isActive()
						|| !entities[mEntity]->hasGroup(i);
				}), group.end());
			auto& m_group(groupedLinkEntities[i]);
			m_group.erase(std::remove_if(std::begin(m_group), std::end(m_group),
				[this, &links_toBeRemoved, i](EntityID mEntity) {
					return !entities[mEntity]->isActive()
						|| !entities[mEntity]->hasGroup(i);
				}), m_group.end());
		}
	}



	visible_emptyEntities.erase(std::remove_if(visible_emptyEntities.begin(), visible_emptyEntities.end(),
		[&toBeRemoved](EntityID mEntity) {
			return std::find(toBeRemoved.begin(), toBeRemoved.end(), mEntity) != toBeRemoved.end();
		}),
		visible_emptyEntities.end());
	visible_nodes.erase(std::remove_if(visible_nodes.begin(), visible_nodes.end(),
		[&nodes_toBeRemoved](EntityID mEntity) {
			return std::find(nodes_toBeRemoved.begin(), nodes_toBeRemoved.end(), mEntity) != nodes_toBeRemoved.end();
		}),
		visible_nodes.end());

	visible_links.erase(std::remove_if(visible_links.begin(), visible_links.end(),
		[&links_toBeRemoved](EntityID mEntity) {
			return std::find(links_toBeRemoved.begin(), links_toBeRemoved.end(), mEntity) != links_toBeRemoved.end();
		}),
		visible_links.end());

	std::vector<EntityID> idsToRemove;
	idsToRemove.reserve(
		toBeRemoved.size() + nodes_toBeRemoved.size() + links_toBeRemoved.size()
	);

	// collect all ids
	for (auto e : toBeRemoved)
		idsToRemove.push_back(e);
	for (auto e : nodes_toBeRemoved)
		idsToRemove.push_back(e);
	for (auto e : links_toBeRemoved)
		idsToRemove.push_back(e);

	// remove by id
	{
		std::unique_lock lock(entities_mtx);
		for (EntityID id : idsToRemove)
			entities.erase(id);
	}
}

void Manager::updateVisibleEntities() {
	visible_emptyEntities = grid->getGridLevel() ? getRevealedEntitiesInCameraCells<EmptyEntity>() : getEntitiesInCameraCells<EmptyEntity>();
	visible_nodes = grid->getGridLevel() ? getRevealedEntitiesInCameraCells<NodeEntity>() : getEntitiesInCameraCells<NodeEntity>();
	visible_links = grid->getLinksInCameraCells();

	for (auto& vgroup : visible_groupedEmptyEntities) {
		vgroup.clear();
	}
	for (auto& vgroup : visible_groupedNodeEntities) {
		vgroup.clear();
	}
	for (auto& vgroup : visible_groupedLinkEntities) {
		vgroup.clear();
	}

	for (auto ventityId : visible_emptyEntities) {
		auto* ent = getEntityFromId(ventityId);
		if (!ent->isActive()) {
			continue;
		}

		for (unsigned i = 0; i < maxGroups; ++i) {
			if (ent->hasGroup(i)) {
				visible_groupedEmptyEntities[i].push_back(ventityId);
			}
		}
	}
	for (auto ventityId : visible_nodes) {
		auto* ent = getEntityFromId(ventityId);
		if (!ent->isActive()) {
			continue;
		}

		for (unsigned i = 0; i < maxGroups; ++i) {
			if (ent->hasGroup(i)) {
				visible_groupedNodeEntities[i].push_back(ventityId);
			}
		}
	}
	for (auto ventityId : visible_links) {
		auto* ent = getEntityFromId(ventityId);
		if (!ent->isActive()) {
			continue;
		}

		for (unsigned i = 0; i < maxGroups; ++i) {
			if (ent->hasGroup(i)) {
				visible_groupedLinkEntities[i].push_back(ventityId);
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

std::vector<EntityID> Manager::collectEntities(
	std::initializer_list<Manager::groupLabels> groupNames,
	Taz::EntityType type
)
{
	std::vector<EntityID> result;

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
std::vector<EntityID> Manager::getRevealedEntitiesInCameraCells() {
	std::vector<EntityID> result;

	if constexpr (std::is_same_v<T, NodeEntity>) {
		for (auto& cell : grid->interceptedCells) {
			for (auto& entityId : cell->nodes) {
				auto* ent = getEntityFromId(entityId);

				if (!ent->isHidden()) {  // Check if the entity is visible
					result.push_back(entityId);

					for (auto& port : ent->children) {
						if (hasEntity(port.second) && !getEntityFromId(port.second)->isHidden()) {
							visible_emptyEntities.push_back(port.second);

							if (getEntityFromId(port.second)->hasComponent<PortComponent>()) {
								for (auto& portSlots : getEntityFromId(port.second)->children)
									visible_emptyEntities.push_back(portSlots.second);
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
					result.push_back(entityId);
				}
			}
		}
	}
	else if constexpr (std::is_same_v<T, LinkEntity>) {
		std::map<unsigned int, LinkEntity*> uniqueEntities;

		for (auto& cell : grid->interceptedCells) {
			for (auto& link : cell->links) {
				if (!link->isHidden()) {
					unsigned int linkId = link->getId();

					if (uniqueEntities.find(linkId) == uniqueEntities.end()) {
						uniqueEntities[linkId] = link;
					}
				}
			}
		}
		for (auto& entry : uniqueEntities) {
			result.push_back(entry.second);
		}
	}
	else {
		static_assert(sizeof(T) == 0, "Unsupported entity type.");
	}
	return result;

}

// loops through the intrecepted cells and just get the entities
template <typename T>
std::vector<EntityID> Manager::getEntitiesInCameraCells() {
	std::vector<EntityID> result;

	if constexpr (std::is_same_v<T, NodeEntity>) {
		for (auto& cell : grid->interceptedCells) {
			result.insert(result.end(), cell->nodes.begin(), cell->nodes.end());
		}

		for (auto& cell : grid->interceptedCells) {
			for (auto& entityID : cell->nodes) {
				auto* entity = getEntityFromId(entityID);
				if (!entity->isHidden()) {
					// Also include children(ports) if they exist
					for (auto& port : entity->children) {
						if (hasEntity(port.second) && !getEntityFromId(port.second)->isHidden()) {
							visible_emptyEntities.push_back(port.second);

							if (getEntityFromId(port.second)->hasComponent<PortComponent>()) {
								for (auto& portSlots : getEntityFromId(port.second)->children)
									visible_emptyEntities.push_back(portSlots.second);
							}
						}
					}
				}
			}
		}

	}
	else if constexpr (std::is_same_v<T, EmptyEntity>) {
		for (auto& cell : grid->interceptedCells) {
			result.insert(result.end(), cell->emptyEntities.begin(), cell->emptyEntities.end());
		}
	}
	else {
		static_assert(sizeof(T) == 0, "Unsupported entity type.");
	}
	return result;
}
