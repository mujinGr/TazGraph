#include "GECSManager.h"

void Manager::updateActiveEntities() {
	std::vector<EntityID> toBeRemoved;
	std::vector<EntityID> nodes_toBeRemoved;
	std::vector<EntityID> links_toBeRemoved;

	for (auto v_entityId : grid->visible_emptyEntities)
	{
		auto* ent = getEntityFromId(v_entityId);
		if (ent->isActive()) {
			ent->removeFromCell();
			toBeRemoved.push_back(v_entityId);
		}
	}

	for (auto& v_entityId : grid->visible_nodes)
	{
		auto* ent = getEntityFromId(v_entityId);
		if (!ent->isActive()) {
			ent->removeFromCell();
			nodes_toBeRemoved.push_back(v_entityId);
		}
	}

	for (auto v_entityId : grid->visible_links)
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

	for (auto i(0u); i < maxGroups; i++) {
		auto& group(visible_groupedEmptyEntities[i]);
		group.erase(std::remove_if(std::begin(group), std::end(group),
			[&toBeRemoved, i](Entity* mEntity) {
				return !mEntity->isActive() || !mEntity->hasGroup(i);
			}), group.end());
		auto& m_group(groupedEmptyEntities[i]);
		m_group.erase(std::remove_if(std::begin(m_group), std::end(m_group),
			[&toBeRemoved, i](Entity* mEntity) {
				return !mEntity->isActive() || !mEntity->hasGroup(i);
			}), m_group.end());
	}

	for (auto i(0u); i < maxGroups; i++) {
		auto& group(visible_groupedNodeEntities[i]);
		group.erase(std::remove_if(std::begin(group), std::end(group),
			[&nodes_toBeRemoved, i](Entity* mEntity) {
				return !mEntity->isActive() || !mEntity->hasGroup(i);
			}), group.end());
		auto& m_group(groupedNodeEntities[i]);
		m_group.erase(std::remove_if(std::begin(m_group), std::end(m_group),
			[&nodes_toBeRemoved, i](Entity* mEntity) {
				return !mEntity->isActive() || !mEntity->hasGroup(i);
			}), m_group.end());
	}

	for (auto i(0u); i < maxGroups; i++) {
		auto& group(visible_groupedLinkEntities[i]);
		group.erase(std::remove_if(std::begin(group), std::end(group),
			[&links_toBeRemoved, i](Entity* mEntity) {
				return !mEntity->isActive() || !mEntity->hasGroup(i);
			}), group.end());
		auto& m_group(groupedLinkEntities[i]);
		m_group.erase(std::remove_if(std::begin(m_group), std::end(m_group),
			[&links_toBeRemoved, i](Entity* mEntity) {
				return !mEntity->isActive() || !mEntity->hasGroup(i);
			}), m_group.end());
	}


	grid->visible_emptyEntities.erase(std::remove_if(grid->visible_emptyEntities.begin(), grid->visible_emptyEntities.end(),
		[&toBeRemoved](Entity* mEntity) {
			return std::find(toBeRemoved.begin(), toBeRemoved.end(), mEntity) != toBeRemoved.end();
		}),
		grid->visible_emptyEntities.end());
	grid->visible_nodes.erase(std::remove_if(grid->visible_nodes.begin(), grid->visible_nodes.end(),
		[&nodes_toBeRemoved](Entity* mEntity) {
			return std::find(nodes_toBeRemoved.begin(), nodes_toBeRemoved.end(), mEntity) != nodes_toBeRemoved.end();
		}),
		grid->visible_nodes.end());

	grid->visible_links.erase(std::remove_if(grid->visible_links.begin(), grid->visible_links.end(),
		[&links_toBeRemoved](Entity* mEntity) {
			return std::find(links_toBeRemoved.begin(), links_toBeRemoved.end(), mEntity) != links_toBeRemoved.end();
		}),
		grid->visible_links.end());

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
		std::scoped_lock lock(entities_mtx);
		for (EntityID id : idsToRemove)
			entities.erase(id);
	}
}

void Manager::updateVisibleEntities() {
	grid->visible_emptyEntities = grid->getGridLevel() ? grid->getRevealedEntitiesInCameraCells<EmptyEntity>() : grid->getEntitiesInCameraCells<EmptyEntity>();
	grid->visible_nodes = grid->getGridLevel() ? grid->getRevealedEntitiesInCameraCells<NodeEntity>() : grid->getEntitiesInCameraCells<NodeEntity>();
	grid->visible_links = grid->getLinksInCameraCells();

	for (auto& vgroup : visible_groupedEmptyEntities) {
		vgroup.clear();
	}
	for (auto& vgroup : visible_groupedNodeEntities) {
		vgroup.clear();
	}
	for (auto& vgroup : visible_groupedLinkEntities) {
		vgroup.clear();
	}

	for (auto* ventity : grid->visible_emptyEntities) {
		if (!ventity->isActive()) {
			continue;
		}

		for (unsigned i = 0; i < maxGroups; ++i) {
			if (ventity->hasGroup(i)) {
				visible_groupedEmptyEntities[i].push_back(ventity);
			}
		}
	}
	for (auto* ventity : grid->visible_nodes) {
		if (!ventity->isActive()) {
			continue;
		}

		for (unsigned i = 0; i < maxGroups; ++i) {
			if (ventity->hasGroup(i)) {
				visible_groupedNodeEntities[i].push_back(ventity);
			}
		}
	}
	for (auto* vlink : grid->visible_links) {
		if (!vlink->isActive()) {
			continue;
		}

		for (unsigned i = 0; i < maxGroups; ++i) {
			if (vlink->hasGroup(i)) {
				visible_groupedLinkEntities[i].push_back(vlink);
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

	const std::string folderPath = fs::current_path().string() + "/../TazGraphEngine/GECS";
	if (!fs::exists(folderPath)) {
		std::cerr << "Error: Folder does not exist at " << folderPath << std::endl;
	}
	const std::string folderPath2 = fs::current_path().string() + "/Src/GECS";
	if (!fs::exists(folderPath)) {
		std::cerr << "Error: Folder does not exist at " << folderPath2 << std::endl;
	}
	scanComponentNames(folderPath);
	scanComponentNames(folderPath2);

}
