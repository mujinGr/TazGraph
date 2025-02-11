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

	bool _update_active_entities = false;
public:
	std::unique_ptr<Grid> grid;

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

		if (_update_active_entities) {
			_update_active_entities = false;

			for (auto& visible_entities : { std::ref(visible_nodes), std::ref(visible_links) })
			{
				for (const auto& v_entity : visible_entities.get()) {
					if (!v_entity->isActive()) {
						v_entity->removeFromCell();
						toBeRemoved.push_back(v_entity);
					}
				}
			}
			// if from visible entities is something deleted, then delete it from all data structures (groupedEntities + enentities)
			// ! or instead of updating the groupedEntities when we see an inactive entity, update the groupedEntities the moment an entity goes
			// ! inactive and wait until we about to delete more
			for (auto& group : groupedEntities) {
				for (Entity* entity : group) {
					if (!entity->isActive()) {
						entity->removeFromCell();
						toBeRemoved.push_back(entity);
					}
				}
			}


			for (auto i(0u); i < maxGroups; i++) {
				auto& group(visible_groupedEntities[i]);
				group.erase(std::remove_if(std::begin(group), std::end(group),
					[&toBeRemoved, i](Entity* mEntity) {
						return !mEntity->isActive() || !mEntity->hasGroup(i);
					}), group.end());
			}

			for (auto& visible_entities : { std::ref(visible_nodes), std::ref(visible_links) }) {
				visible_entities.get().erase(std::remove_if(visible_entities.get().begin(), visible_entities.get().end(),
					[&toBeRemoved](Entity* mEntity) {
						return std::find(toBeRemoved.begin(), toBeRemoved.end(), mEntity) != toBeRemoved.end();
					}),
					visible_entities.get().end());
			}
		}
		

		std::vector<Cell*> intercepted_cells = grid->getIntersectedCameraCells(*camera, this->grid->getGridLevel());

		if (grid->hasCellsChanged(intercepted_cells) || grid->gridLevelChanged()) {
			grid->updateLastInterceptedCells(intercepted_cells);
		}
		else {
			return;
		}

		visible_nodes = grid->getGridLevel() ? grid->getRevealedNodesInCameraCells(intercepted_cells) : grid->getNodesInCameraCells(intercepted_cells);
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

	void updateActiveEntities() {
		_update_active_entities = true;
	}

	void AddToGroup(Entity* mEntity, Group mGroup)
	{
		groupedEntities[mGroup].emplace_back(mEntity);
	}

	std::vector<Entity*> getEntities() const {
		std::vector<Entity*> allEntities;
		allEntities.reserve(entities.size());
		for (const auto& entity : entities) {
			allEntities.push_back(entity.get());
		}
		return allEntities;
	}

	std::vector<Entity*> getVisibleNodes() {
		return visible_nodes;
	}

	std::vector<Entity*> getVisibleLinks() {
		return visible_links;
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
	T& addEntityWithId(unsigned int id, TArgs&&... mArgs)
	{
		T* e(new T(*this, std::forward<TArgs>(mArgs)...));
		e->setId(id);
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

	std::string getGroupName(Group mGroup) const {
		return groupNames.at(mGroup);
	}
	
	enum layerIndexes : int
	{
		actionLayer = 0,

		menubackgroundLayer = 10000,
	};

};