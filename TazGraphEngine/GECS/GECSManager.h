#pragma once

#include "GECS.h"
#include "../Grid/Grid.h"

class Manager
{
private:
	unsigned int lastEntityId = 0;
	std::vector<std::unique_ptr<Entity>> entities;
	std::array<std::vector<Entity*>, maxGroups> groupedEntities;

	std::vector<Entity*> visible_entities;
	std::array<std::vector<Entity*>, maxGroups> visible_groupedEntities;


public:
	std::unique_ptr<Grid> grid;

	void update(float deltaTime = 1.0f, ICamera* camera = nullptr)
	{
		if (grid) {
			for (Entity* e : visible_entities) {
				if (!e || !e->isActive()) continue;
				e->update(deltaTime);

				//check if entity that has cell has to change cell
				if (e->ownerCell) {
					Cell* newCell = grid->getCell(*e);
					if (newCell != e->ownerCell) {
						// Need to shift the entity
						grid->removeEntity(e);
						grid->addEntity(e, newCell);
					}
				}
			}
		}
		else {
			for (auto& e : entities) {
				if (!e || !e->isActive()) continue;
				e->update(deltaTime);

				//check if entity that has cell has to change cell
				if (e->ownerCell) {
					Cell* newCell = grid->getCell(*e);
					if (newCell != e->ownerCell) {
						// Need to shift the entity
						grid->removeEntity(e.get());
						grid->addEntity(e.get(), newCell);
					}
				}
			}
		}
	}

	void updateFully(float deltaTime = 1.0f)
	{
		for (auto& e : entities) {
			if (!e || !e->isActive()) continue;
			e->updateFully(deltaTime);

			//check if entity that has cell has to change cell
			if (e->ownerCell) {
				Cell* newCell = grid->getCell(*e);
				if (newCell != e->ownerCell) {
					// Need to shift the entity
					grid->removeEntity(e.get());
					grid->addEntity(e.get(), newCell);
				}
			}
		}
	}

	void refresh(ICamera* camera = nullptr)
	{
		if (grid) {
			visible_entities = grid->getEntitiesInCameraCells(*camera);

			for (auto i(0u); i < maxGroups; i++)
			{
				visible_groupedEntities[i].clear();
				std::copy_if(visible_entities.begin(), visible_entities.end(), std::back_inserter(visible_groupedEntities[i]),
					[this, i](Entity* entity) {
						return entity->hasGroup(i) && entity->isActive();
					});

				visible_groupedEntities[i].erase(
					std::remove_if(std::begin(visible_groupedEntities[i]), std::end(visible_groupedEntities[i]),
						[this, i](Entity* mEntity)
						{
							return !mEntity->isActive() || !mEntity->hasGroup(i);
						}),
					std::end(visible_groupedEntities[i]));
			}
		}
		else {
			for (auto i(0u); i < maxGroups; i++)
			{
				auto& v(groupedEntities[i]);
				v.erase(
					std::remove_if(std::begin(v), std::end(v),
						[this, i](Entity* mEntity)
						{
							return !mEntity->isActive() || !mEntity->hasGroup(i);
						}),
					std::end(v));
			}
		}
		


		//entities.erase(std::remove_if(std::begin(entities), std::end(entities),
		//	[this](const std::unique_ptr<Entity>& mEntity)
		//	{
		//		if (!mEntity->isActive()) {
		//			if (mEntity->ownerCell) {
		//				grid->removeEntity(mEntity.get());  // Remove entity from the grid
		//				mEntity->ownerCell = nullptr;
		//			}
		//			return true;
		//		}
		//		return false;
		//	}),
		//	std::end(entities));
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

	Entity& addEntity()
	{
		Entity* e = new Entity(*this);
		e->setId(lastEntityId++);
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

	std::unordered_map<Group, std::string> groupNames = {
		{groupBackgroundLayer, "groupBackgroundLayer" },
		{panelBackground, "panelBackground"},

		//action
		{ groupLinks_0,"groupLinks_0" },
		{ groupNodes_0,"groupNodes_0" },
		{ groupLinks,"groupLinks" },
		{ groupColliders,"groupColliders" },

		//fore
		{ buttonLabels,"buttonLabels" },
		{ cursorGroup,"cursorGroup" }
	};

	std::string getGroupName(Group mGroup) {
		return groupNames[mGroup];
	}
	
	enum layerIndexes : int
	{
		actionLayer = 0,

		menubackgroundLayer = 10000,
	};

};