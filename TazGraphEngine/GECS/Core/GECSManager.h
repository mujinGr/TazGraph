#pragma once

#include "GECS.h"
#include "../../Grid/Grid.h"

class Manager
{
private:
	int lastEntityId = 0;
	int negativeEntityId = -1;
	std::vector<std::unique_ptr<Entity>> entities;

	std::array<std::vector<EmptyEntity*>, maxGroups> groupedEmptyEntities;
	std::array<std::vector<NodeEntity*>, maxGroups> groupedNodeEntities;
	std::array<std::vector<LinkEntity*>, maxGroups> groupedLinkEntities;

	std::vector<NodeEntity*> visible_emptyEntities;
	std::vector<NodeEntity*> visible_nodes;
	std::vector<LinkEntity*> visible_links;
	
	std::array<std::vector<EmptyEntity*>, maxGroups> visible_groupedEmptyEntities;
	std::array<std::vector<NodeEntity*>, maxGroups> visible_groupedNodeEntities;
	std::array<std::vector<LinkEntity*>, maxGroups> visible_groupedLinkEntities;

	bool _update_active_entities = false;
public:
	std::unique_ptr<Grid> grid;

	void update(float deltaTime = 1.0f)
	{
		for (auto& e : visible_nodes) {
			if (!e || !e->isActive()) continue;
			
			e->update(deltaTime);
		}

		for (auto& e : visible_links) {
			if (!e || !e->isActive()) continue;

			e->update(deltaTime);
		}
	}

	// update fully will update all nodes and links in the world
	void updateFully(float deltaTime = 1.0f)
	{
		// the links are updating once since after first update we check wether the nodes are aligned with the ownerCells
		for (auto& e : entities) { 
			if (!e || !e->isActive()) continue;
			
			e->update(deltaTime);
		}
	}

	void refresh(ICamera* camera = nullptr)
	{
	

		if (_update_active_entities) {
			_update_active_entities = false;

			updateActiveEntities();
		}
		

		if (camera->hasChanged()) {
			bool interceptedCellsChanged = grid->setIntersectedCameraCells(*camera);

			if (!interceptedCellsChanged && !grid->gridLevelChanged()) {
				return;
			}

			updateVisibleEntities();
		}		
	}

	void aboutTo_updateActiveEntities() {
		_update_active_entities = true;
	}

	void updateActiveEntities() {
		std::vector<NodeEntity*> toBeRemoved;
		std::vector<LinkEntity*> links_toBeRemoved;

		for (auto& v_entity : visible_emptyEntities)
		{
			if (!v_entity->isActive()) {
				v_entity->removeFromCell();
				toBeRemoved.push_back(v_entity);
			}
		}

		for (auto& v_entity : visible_nodes)
		{
			if (!v_entity->isActive()) {
				v_entity->removeFromCell();
				toBeRemoved.push_back(v_entity);
			}
		}

		for (auto& v_entity : visible_links)
		{
			if (!v_entity->isActive()) {
				v_entity->removeFromCells();
				links_toBeRemoved.push_back(v_entity);
			}
		}
		// if from visible entities is something deleted, then delete it from all data structures (groupedEntities + enentities)
		// ! or instead of updating the groupedEntities when we see an inactive entity, update the groupedEntities the moment an entity goes
		// ! inactive and wait until we about to delete more
		for (auto& group : groupedNodeEntities) {
			for (NodeEntity* entity : group) {
				if (!entity->isActive()) {
					entity->removeFromCell();
					toBeRemoved.push_back(entity);
				}
			}
		}
		for (auto& group : groupedLinkEntities) {
			for (LinkEntity* entity : group) {
				if (!entity->isActive()) {
					entity->removeFromCells();
					links_toBeRemoved.push_back(entity);
				}
			}
		}


		for (auto i(0u); i < maxGroups; i++) {
			auto& group(visible_groupedNodeEntities[i]);
			group.erase(std::remove_if(std::begin(group), std::end(group),
				[&toBeRemoved, i](Entity* mEntity) {
					return !mEntity->isActive() || !mEntity->hasGroup(i);
				}), group.end());
		}

		for (auto i(0u); i < maxGroups; i++) {
			auto& group(visible_groupedLinkEntities[i]);
			group.erase(std::remove_if(std::begin(group), std::end(group),
				[&links_toBeRemoved, i](Entity* mEntity) {
					return !mEntity->isActive() || !mEntity->hasGroup(i);
				}), group.end());
		}

		visible_nodes.erase(std::remove_if(visible_nodes.begin(), visible_nodes.end(),
			[&toBeRemoved](Entity* mEntity) {
				return std::find(toBeRemoved.begin(), toBeRemoved.end(), mEntity) != toBeRemoved.end();
			}),
			visible_nodes.end());
		
		visible_links.erase(std::remove_if(visible_links.begin(), visible_links.end(),
			[&links_toBeRemoved](Entity* mEntity) {
				return std::find(links_toBeRemoved.begin(), links_toBeRemoved.end(), mEntity) != links_toBeRemoved.end();
			}),
			visible_links.end());
	
	}

	void updateVisibleEntities() {
		visible_nodes = grid->getGridLevel() ? grid->getRevealedNodesInCameraCells() : grid->getEntitiesInCameraCells<NodeEntity>();
		visible_links = grid->getLinksInCameraCells();

		for (auto& vgroup : visible_groupedNodeEntities) {
			vgroup.clear();
		}
		for (auto& vgroup : visible_groupedLinkEntities) {
			vgroup.clear();
		}

		for (auto* ventity : visible_nodes) {
			if (!ventity->isActive()) {
				continue;
			}

			for (unsigned i = 0; i < maxGroups; ++i) {
				if (ventity->hasGroup(i)) {
					visible_groupedNodeEntities[i].push_back(ventity);
				}
			}
		}
		for (auto* vlink : visible_links) {
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

	void AddToGroup(EmptyEntity* mEntity, Group mGroup)
	{
		groupedEmptyEntities[mGroup].emplace_back(mEntity);
	}

	void AddToGroup(NodeEntity* mEntity, Group mGroup)
	{
		groupedNodeEntities[mGroup].emplace_back(mEntity);
	}

	void AddLinkToGroup(LinkEntity* mEntity, Group mGroup)
	{
		groupedLinkEntities[mGroup].emplace_back(mEntity);
	}

	const std::vector<std::unique_ptr<Entity>>& getEntities() const {
		return entities;
	}

	std::vector<NodeEntity*> getVisibleNodes() {
		return visible_nodes;
	}

	std::vector<LinkEntity*> getVisibleLinks() {
		return visible_links;
	}

	std::vector<NodeEntity*>& getGroup(Group mGroup)
	{
		return groupedNodeEntities[mGroup];
	}

	std::vector<NodeEntity*>& getVisibleGroup(Group mGroup)
	{
		return visible_groupedNodeEntities[mGroup];
	}
	// todo make it template
	std::vector<LinkEntity*>& getLinkGroup(Group mGroup)
	{
		return groupedLinkEntities[mGroup];
	}

	std::vector<LinkEntity*>& getVisibleLinkGroup(Group mGroup)
	{
		return visible_groupedLinkEntities[mGroup];
	}

	template <typename T, typename... TArgs>
	T& addEntityNoId(TArgs&&... mArgs)
	{
		T* e(new T(*this, std::forward<TArgs>(mArgs)...));
		e->setId(negativeEntityId--);
		std::unique_ptr<T> uPtr{ e };
		entities.emplace_back(std::move(uPtr));

		return *e;
	}

	template <typename T, typename... TArgs>
	T& addEntity(TArgs&&... mArgs)
	{
		T* e(new T(*this, std::forward<TArgs>(mArgs)...));
		e->setId(lastEntityId++);
		std::unique_ptr<T> uPtr{ e };
		entities.emplace_back(std::move(uPtr));

		return *e;
	}

	void resetEntityId() {
		lastEntityId = 0;
	}

	Entity* getEntityFromId(unsigned int mId) {
		for (auto& entity : entities) {
			if (entity->getId() == mId && entity->isActive()) {
				return &*entity;
			}
		}
		return nullptr;
	}

	void clearAllEntities() {
		for (auto& group : groupedNodeEntities) {
			group.clear();
		}
		for (auto& group : groupedLinkEntities) {
			group.clear();
		}
		entities.clear();
	}

	void removeAllEntites() {
		for (std::size_t group = Manager::groupBackgroundLayer; group != Manager::cursorGroup; group++) {
			removeAllEntitiesFromGroup(group);
			removeAllEntitiesFromLinkGroup(group);
		}
	}

	void removeAllEntitiesFromGroup(Group mGroup) {
		auto& entitiesInGroup = groupedNodeEntities[mGroup];

		for (Entity* entity : entitiesInGroup) {
			entity->destroy();
		}
	}
	void removeAllEntitiesFromLinkGroup(Group mGroup) {
		auto& entitiesInGroup = groupedLinkEntities[mGroup];

		for (Entity* entity : entitiesInGroup) {
			entity->destroy();
		}
	}

	std::vector<Entity*> adjacentEntities(Entity* mainEntity, Group group) {
		std::vector<Entity*> nearbyEntities;

		// get entities of adjacent cells and return those that belong to group, and append them to result
		auto adjacentCells = grid->getAdjacentCells(*mainEntity, grid->getGridLevel());

		for (Cell* adjCell : adjacentCells) {
			for (auto& neighbor : adjCell->nodes) {
				if (neighbor->hasGroup(group)) { // Optional: Exclude the original entity if necessary
					nearbyEntities.push_back(neighbor);
				}
			}
		}
		
		return nearbyEntities;
	}

	enum groupLabels : std::size_t //todo should add groups at end for some reason
	{
		//back
		groupBackgroundLayer,
		panelBackground,

		//action
		groupLinks_0,
		groupGroupLinks_0,
		groupGroupLinks_1,

		groupArrowHeads_0,

		groupNodes_0,
		groupGroupNodes_0,
		groupGroupNodes_1,
		groupColliders,
		
		//fore
		buttonLabels,
		cursorGroup
	};

	const std::unordered_map<Group, std::string> groupNames = {
		{groupBackgroundLayer, "groupBackgroundLayer" },
		{panelBackground, "panelBackground"},

		//action
		{ groupLinks_0,"groupLinks_0" },
		{groupGroupLinks_0, "groupGroupLinks_0"},
		{groupGroupLinks_1, "groupGroupLinks_1"},

		{groupArrowHeads_0, "groupArrowHeads_0"},

		{ groupNodes_0,"groupNodes_0" },
		{ groupGroupNodes_0, "groupGroupNodes_0"},
		{ groupGroupNodes_1, "groupGroupNodes_1"},

		{ groupColliders,"groupColliders" },

		//fore
		{ buttonLabels,"buttonLabels" },

		//globals, dont remove
		{ cursorGroup,"cursorGroup" }
	};

	std::string getGroupName(Group mGroup) const;

};