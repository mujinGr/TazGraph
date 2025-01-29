#pragma once

#include "GECS.h"
#include "../Grid/Grid.h"

class Manager
{
private:
	int lastEntityId = 0;
	std::vector<std::unique_ptr<Entity>> entities;
	std::array<std::vector<Entity*>, maxGroups> groupedEntities;

	std::vector<Entity*> visible_entities;
	std::vector<Entity*> visible_links;
	std::array<std::vector<Entity*>, maxGroups> visible_groupedEntities;

public:
	std::unique_ptr<Grid> grid;

	bool entitiesAreGrouped = false;

	void update(float deltaTime = 1.0f)
	{
		for (auto& e : visible_entities) {
			if (!e || !e->isActive()) continue;
			e->update(deltaTime);
			// for all connected links update
			e->cellUpdate();
		}
		for (auto& le : visible_links) {
			if (!le || !le->isActive()) continue;
			le->update(deltaTime);

			le->cellUpdate();
		}
		
	}

	void updateFully(float deltaTime = 1.0f)
	{
		for (auto& e : entities) { //todo since links depend on nodes, then nodes first update and then links. To make it correctly link should be as var in node
			if (!e || !e->isActive()) continue;
			e->updateFully(deltaTime);

			e->cellUpdate();
		}
	}

	void refresh(ICamera* camera = nullptr)
	{
		
		entities.erase(std::remove_if(std::begin(entities), std::end(entities),
			[this](const std::unique_ptr<Entity>& mEntity)
			{
				if (!mEntity->isActive()) {
					mEntity->removeFromCell();

					return true;
				}
				return false;
			}),
			std::end(entities));

		std::vector<Cell*> intercepted_cells = grid->getIntercectedCameraCells(*camera);

		visible_entities = entitiesAreGrouped ? grid->getVisibleEntitiesInCameraCells(intercepted_cells) : grid->getEntitiesInCameraCells(intercepted_cells);
		visible_links	 = grid->getLinksInCameraCells(intercepted_cells);

		for (auto& vgroup : visible_groupedEntities) {
			vgroup.clear();
		}

		for (auto* ventity : visible_entities) {
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
			for (auto& neighbor : adjCell->entities) {
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
		groupNodes_0,
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
		{ groupNodes_0,"groupNodes_0" },
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