#pragma once

#include "GECS.h"
#include "../Grid/Grid.h"

class Manager
{
private:
	int lastEntityId = 0;
	std::vector<std::unique_ptr<Entity>> entities;
	std::array<std::vector<Entity*>, maxGroups> groupedEntities;

	std::vector<Entity*> visible_nodes;
	std::vector<Entity*> visible_links;
	std::array<std::vector<Entity*>, maxGroups> visible_groupedEntities;

public:
	std::unique_ptr<Grid> grid;

	bool entitiesAreGrouped = false;

	void update(float deltaTime = 1.0f)
	{
		for (auto& e : visible_nodes) {
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
	
		std::vector<Entity*> toBeRemoved;

		for (const auto& entity : entities) {
			if (!entity->isActive()) {
				entity->removeFromCell();
				toBeRemoved.push_back(entity.get());
			}
		}

		for (auto i(0u); i < maxGroups; i++) {
			auto& group(groupedEntities[i]);
			group.erase(std::remove_if(std::begin(group), std::end(group),
				[&toBeRemoved, i](Entity* mEntity) {
					return !mEntity->isActive() || !mEntity->hasGroup(i);
				}), group.end());
		}

		entities.erase(std::remove_if(std::begin(entities), std::end(entities),
			[&toBeRemoved](const std::unique_ptr<Entity>& mEntity) {
				return std::find(toBeRemoved.begin(), toBeRemoved.end(), mEntity.get()) != toBeRemoved.end();
			}),
			std::end(entities));

		std::vector<Cell*> intercepted_cells = grid->getIntercectedCameraCells(*camera);

		visible_nodes = entitiesAreGrouped ? grid->getRevealedNodesInCameraCells(intercepted_cells) : grid->getNodesInCameraCells(intercepted_cells);
		visible_links	 = grid->getLinksInCameraCells(intercepted_cells);

		for (auto& vgroup : visible_groupedEntities) {
			vgroup.clear();
		}

		for (auto* ventity : visible_nodes) {
			if (!ventity->isActive()) {
				continue; 
			}

			for (unsigned i = 0; i < maxGroups; ++i) {
				if (ventity->hasGroup(i)) {
					visible_groupedEntities[i].push_back(ventity);
				}
			}
		}
		for (auto* vlink : visible_links) {
			if (!vlink->isActive()) {
				continue;
			}

			for (unsigned i = 0; i < maxGroups; ++i) {
				if (vlink->hasGroup(i)) {
					visible_groupedEntities[i].push_back(vlink);
				}
			}
		}
		
	}

	void AddToGroup(Entity* mEntity, Group mGroup)
	{
		groupedEntities[mGroup].emplace_back(mEntity);
	}

	std::vector<Entity*>& getGroup(Group mGroup)
	{
		return groupedEntities[mGroup];
	}

	std::vector<Entity*>& getVisibleGroup(Group mGroup)
	{
		return visible_groupedEntities[mGroup];
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

	Entity& addEntity()
	{
		return addEntity(lastEntityId++);
	}

	Entity& addEntity(int id)
	{
		Entity* e = new Entity(*this);
		e->setId(id);
		std::unique_ptr<Entity> uPtr{ e };
		entities.emplace_back(std::move(uPtr));

		return *e;
	}

	Entity& getEntityFromId(unsigned int mId) {
		for (auto& entity : entities) {
			if (entity->getId() == mId && entity->isActive()) {
				return *entity;
			}
		}
	}

	void clearAllEntities() {
		for (auto& group : groupedEntities) {
			group.clear();
		}
		entities.clear();
	}

	void removeAllEntites() {
		for (std::size_t group = Manager::groupBackgroundLayer; group != Manager::cursorGroup; group++) {
			removeAllEntitiesFromGroup(group);
		}
	}

	void removeAllEntitiesFromGroup(Group mGroup) {
		auto& entitiesInGroup = groupedEntities[mGroup];

		for (Entity* entity : entitiesInGroup) {
			entity->destroy();
		}
	}

	std::vector<Entity*> adjacentEntities(Entity* mainEntity, Group group) {
		std::vector<Entity*> nearbyEntities;

		// get entities of adjacent cells and return those that belong to group, and append them to result
		auto adjacentCells = grid->getAdjacentCells(*mainEntity);

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

		groupNodes_0,
		groupGroupNodes_0,
		groupLinks,
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

		{ groupNodes_0,"groupNodes_0" },
		{ groupGroupNodes_0, "groupGroupNodes_0"},

		{ groupLinks,"groupLinks" },
		{ groupColliders,"groupColliders" },

		//fore
		{ buttonLabels,"buttonLabels" },

		//globals, dont remove
		{ cursorGroup,"cursorGroup" }
	};

	std::string getGroupName(Group mGroup) const {
		return groupNames.at(mGroup);
	}
	
	enum layerIndexes : int
	{
		actionLayer = 0,

		menubackgroundLayer = 10000,
	};

};