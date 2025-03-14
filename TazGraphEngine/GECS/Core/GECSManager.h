#pragma once

#include "GECS.h"
#include "../../Grid/Grid.h"

#include "../../Threader/Threader.h"

class Manager
{
private:
	//Threader& _threader;
	int lastEntityId = 0;
	int negativeEntityId = -1;
	std::vector<std::unique_ptr<Entity>> entities;

	std::array<std::vector<EmptyEntity*>, maxGroups> groupedEmptyEntities;
	std::array<std::vector<NodeEntity*>, maxGroups> groupedNodeEntities;
	std::array<std::vector<LinkEntity*>, maxGroups> groupedLinkEntities;

	std::vector<EmptyEntity*> visible_emptyEntities;
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
		for (auto& e : visible_emptyEntities) {
			if (!e || !e->isActive()) continue;

			e->update(deltaTime);
		}

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

	void updateActiveEntities();

	void updateVisibleEntities();

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

	template <typename T>
	std::vector<T*> getVisible() {
		return {};
	}

	template <>
	std::vector<EmptyEntity*> getVisible() {
		return visible_emptyEntities;
	}
	template <>
	std::vector<NodeEntity*> getVisible() {
		return visible_nodes;
	}
	template <>
	std::vector<LinkEntity*> getVisible() {
		return visible_links;
	}

	template <typename T>
	std::vector<T*>& getVisibleGroup(Group mGroup) {
		return {};
	}

	template <typename T>
	std::vector<T*>& getGroup(Group mGroup) {
		return {};
	}

	template <>
	std::vector<EmptyEntity*>& getVisibleGroup(Group mGroup)
	{
		return visible_groupedEmptyEntities[mGroup];
	}
	
	template <>
	std::vector<EmptyEntity*>& getGroup(Group mGroup)
	{
		return groupedEmptyEntities[mGroup];
	}

	template <>
	std::vector<NodeEntity*>& getGroup(Group mGroup)
	{
		return groupedNodeEntities[mGroup];
	}

	template <>
	std::vector<NodeEntity*>& getVisibleGroup(Group mGroup)
	{
		return visible_groupedNodeEntities[mGroup];
	}
	
	template <>
	std::vector<LinkEntity*>& getGroup(Group mGroup)
	{
		return groupedLinkEntities[mGroup];
	}

	template <>
	std::vector<LinkEntity*>& getVisibleGroup(Group mGroup)
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
		for (std::size_t group = Manager::groupBackgroundLayer; group != Manager::buttonLabels; group++) {
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
	};

	std::string getGroupName(Group mGroup) const;

};