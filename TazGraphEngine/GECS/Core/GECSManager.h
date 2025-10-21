#pragma once

#include "GECS.h"
#include "../../Grid/Grid.h"

#include "../../Threader/Threader.h"

#include "./SimulationStep.h"

#include <regex>
#include <filesystem>

namespace fs = std::filesystem;


class Manager
{
private:
	std::mutex entities_mtx;

	Threader* _threader = nullptr;
	int lastEntityId = 0;
	int negativeEntityId = -1;
	std::unordered_map<EntityID, std::unique_ptr<Entity>> entities;

	std::array<std::vector<EntityID>, maxGroups> groupedEmptyEntities;
	std::array<std::vector<EntityID>, maxGroups> groupedNodeEntities;
	std::array<std::vector<EntityID>, maxGroups> groupedLinkEntities;

	std::vector<EntityID> visible_emptyEntities;
	std::vector<EntityID> visible_nodes;
	std::vector<EntityID> visible_links;

	std::array<std::vector<EntityID>, maxGroups> visible_groupedEmptyEntities;
	std::array<std::vector<EntityID>, maxGroups> visible_groupedNodeEntities;
	std::array<std::vector<EntityID>, maxGroups> visible_groupedLinkEntities;

	bool _update_active_entities = false;
public:
	std::vector<SimulationStep> steps;
	int currentStep = 0;

	std::vector<NodeEntity*> movedNodes;
	std::mutex movedNodesMutex;

	bool idTextEnabled = false;

	bool arrowheadsEnabled = false;
	bool last_arrowheadsEnabled = false;

	bool updateInnerPathLinks = false;

	std::unordered_map<std::string, std::vector<std::string>> componentNames;

	std::unique_ptr<Grid> grid;

	Manager() {}

	~Manager() { _threader = nullptr; }

	void setThreader(Threader& mthreader) {
		_threader = &mthreader;
	}

	void update(float deltaTime = 1.0f)
	{
		//! THREADER CHECK

		if (_threader && !_threader->t_queue.shuttingDown) {

			//! UPDATE LINK CELLS
			//? THIS MAY CAUSE ERRORS, IF REMOVE LINK FROM CELL AND OTHER LINK THAT HAS THAT CELL IN SEARCH
			//? WILL PUMP IN AN EMPTY ELEMENT OR THE SIZE WILL BE SMALLER FOR THAT LINK TO FIND ELEMENT
			for (auto& e : movedNodes) {
				updateInnerPathLinks = true;
				for (auto& link : e->getInLinks()) {
					link->cellUpdate();
				}
				for (auto& link : e->getOutLinks()) {
					link->cellUpdate();
				}
			}

			_threader->parallel(movedNodes.size(), [&](int start, int end) {
				for (int i = start; i < end; i++) {
					for (auto& link : movedNodes[i]->getInLinks()) {
						link->updateConnectedPorts();
					}
				}
				});

			_threader->parallel(movedNodes.size(), [&](int start, int end) {
				for (int i = start; i < end; i++) {
					for (auto& link : movedNodes[i]->getOutLinks()) {
						link->updateConnectedPorts();
					}
				}
				});

			//! UPDATE
			_threader->parallel(visible_emptyEntities.size(), [&](int start, int end) {
				for (int i = start; i < end; i++) {
					if (getEntityFromId(visible_emptyEntities[i])->isActive()) {
						getEntityFromId(visible_emptyEntities[i])->update(deltaTime);
					}
				}
				});


			_threader->parallel(visible_nodes.size(), [&](int start, int end) {
				for (int i = start; i < end; i++) {
					if (getEntityFromId(visible_nodes[i])->isActive()) {
						getEntityFromId(visible_nodes[i])->update(deltaTime);
					}
				}

				});


			_threader->parallel(visible_links.size(), [&](int start, int end) {

				for (int i = start; i < end; i++) {
					if (getEntityFromId(visible_links[i])->isActive()) {
						getEntityFromId(visible_links[i])->update(deltaTime);
					}
				}
				});

		}

		//! FOR MAIN MENU
		else {

			//! CELL UPDATE

			for (auto& e : movedNodes) {
				for (auto& link : e->getInLinks()) {
					link->cellUpdate();
				}
				for (auto& link : e->getOutLinks()) {
					link->cellUpdate();
				}
			}

			for (auto& e : movedNodes) {
				for (auto& link : e->getInLinks()) {
					link->updateConnectedPorts();
				}
			}

			for (auto& e : movedNodes) {
				for (auto& link : e->getOutLinks()) {
					link->updateConnectedPorts();
				}
			}

			movedNodes.clear();

			for (auto& e : visible_emptyEntities) {
				auto* ent = getEntityFromId(e);
				if (!ent->isActive()) continue;

				ent->update(deltaTime);
			}

			if (arrowheadsEnabled) {
				for (auto& e : visible_nodes) {
					auto* ent = getEntityFromId(e);
					if (!ent->isActive()) continue;

					ent->update(deltaTime);

				}
			}


			for (auto& e : visible_links) {
				auto* ent = getEntityFromId(e);
				if (!ent->isActive()) continue;

				ent->update(deltaTime);
			}
		}
	}

	// update fully will update all nodes and links in the world
	void updateFully(float deltaTime = 1.0f)
	{
		// the links are updating once since after first update we check wether the nodes are aligned with the ownerCells
		for (auto& e : entities) {
			if (!e.second || !e.second->isActive()) continue;

			e.second->update(deltaTime);
		}
		update(deltaTime);
	}

	void refresh(ICamera* camera = nullptr)
	{

		if (grid && (camera->hasChanged() || grid->gridLevelChanged())) {
			bool interceptedCellsChanged = grid->setIntersectedCameraCells(*camera);

			if (interceptedCellsChanged) {
				aboutTo_updateActiveEntities();
			}
			camera->refreshCamera();
		}

		if (_update_active_entities) {
			_update_active_entities = false;

			updateActiveEntities();
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
		groupedEmptyEntities[mGroup].emplace_back(mEntity->getId());
	}

	void AddToGroup(NodeEntity* mEntity, Group mGroup)
	{
		groupedNodeEntities[mGroup].emplace_back(mEntity->getId());
	}

	void AddLinkToGroup(LinkEntity* mEntity, Group mGroup)
	{
		groupedLinkEntities[mGroup].emplace_back(mEntity->getId());
	}

	const std::unordered_map<EntityID, std::unique_ptr<Entity>>& getEntities() const {
		return entities;
	}

	template <typename T>
	std::vector<EntityID> getVisible() {
		if constexpr (std::is_same_v<T, EmptyEntity>) {
			return visible_emptyEntities;
		}
		else if constexpr (std::is_same_v<T, NodeEntity>) {
			return visible_nodes;
		}
		else if constexpr (std::is_same_v<T, LinkEntity>) {
			return visible_links;
		}
		else {
			static_assert(sizeof(T) == 0, "Unsupported entity type.");
		}
	}

	template <typename T>
	std::vector<EntityID>& getVisibleGroup(Group mGroup) {
		if constexpr (std::is_same_v<T, EmptyEntity>) {
			return visible_groupedEmptyEntities[mGroup];
		}
		else if constexpr (std::is_same_v<T, NodeEntity>) {
			return visible_groupedNodeEntities[mGroup];
		}
		else if constexpr (std::is_same_v<T, LinkEntity>) {
			return visible_groupedLinkEntities[mGroup];
		}
		else {
			static_assert(sizeof(T) == 0, "Unsupported entity type.");
		}
	}

	template <typename T>
	void getAllTypeEntities(std::vector<EntityID>& output) {
		output.clear();

		if constexpr (std::is_same_v<T, EmptyEntity>) {
			for (auto& groupPair : groupedEmptyEntities) {
				output.insert(output.end(), groupPair.begin(), groupPair.end());
			}
		}
		else if constexpr (std::is_same_v<T, NodeEntity>) {
			for (auto& groupPair : groupedNodeEntities) {
				output.insert(output.end(), groupPair.begin(), groupPair.end());
			}
		}
		else if constexpr (std::is_same_v<T, LinkEntity>) {
			for (auto& groupPair : groupedLinkEntities) {
				output.insert(output.end(), groupPair.begin(), groupPair.end());
			}
		}
		else {
			static_assert(sizeof(T) == 0, "Unsupported entity type.");
		}
	}

	template <typename T>
	std::vector<EntityID>& getGroup(Group mGroup) {
		if constexpr (std::is_same_v<T, EmptyEntity>) {
			return groupedEmptyEntities[mGroup];
		}
		else if constexpr (std::is_same_v<T, NodeEntity>) {
			return groupedNodeEntities[mGroup];
		}
		else if constexpr (std::is_same_v<T, LinkEntity>) {
			return groupedLinkEntities[mGroup];
		}
		else {
			static_assert(sizeof(T) == 0, "Unsupported entity type.");
		}
	}

	template <typename T, typename... TArgs>
	T& addEntityNoId(TArgs&&... mArgs)
	{
		T* e(new T(*this, std::forward<TArgs>(mArgs)...));
		e->setId(negativeEntityId--);
		std::unique_ptr<T> uPtr{ e };
		{
			std::scoped_lock lock(entities_mtx);
			entities.emplace(e->getId(), std::move(uPtr));
		}

		return *e;
	}

	template <typename T, typename... TArgs>
	T& addEntity(TArgs&&... mArgs)
	{
		T* e(new T(*this, std::forward<TArgs>(mArgs)...));
		e->setId(lastEntityId++);
		std::unique_ptr<T> uPtr{ e };
		{
			std::scoped_lock lock(entities_mtx);
			entities.emplace(e->getId(), std::move(uPtr));
		}

		return *e;
	}

	void resetEntityId() {
		lastEntityId = 0;
	}

	inline Entity* getEntityFromId(EntityID mId) {
		return entities[mId].get();
	}

	bool hasEntity(EntityID mId) {
		return entities.contains(mId);
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
		for (auto group : groupNames) {
			removeAllEntitiesFromGroup(group.first);
			removeAllEntitiesFromLinkGroup(group.first);
		}
	}

	void removeAllEntitiesFromGroup(Group mGroup) {
		auto& entitiesInGroup = groupedNodeEntities[mGroup];

		for (auto entityId : entitiesInGroup) {
			getEntityFromId(entityId)->destroy();
		}
	}

	void removeAllEntitiesFromEmptyGroup(Group mGroup) {
		auto& entitiesInGroup = groupedEmptyEntities[mGroup];

		for (auto entityId : entitiesInGroup) {
			getEntityFromId(entityId)->destroy();
		}
	}

	void removeAllEntitiesFromLinkGroup(Group mGroup) {
		auto& entitiesInGroup = groupedLinkEntities[mGroup];

		for (auto entityId : entitiesInGroup) {
			getEntityFromId(entityId)->destroy();
		}
	}

	std::vector<EntityID> adjacentEntities(Entity* mainEntity, Group group) {
		std::vector<EntityID> nearbyEntities;

		auto adjacentCells = grid->getAdjacentCells(*mainEntity, grid->getGridLevel());

		for (Cell* adjCell : adjacentCells) {
			for (auto& neighbor : adjCell->nodes) {
				if (getEntityFromId(neighbor)->hasGroup(group) && (getEntityFromId(neighbor) != mainEntity)) {
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

		groupPathLinks,

		groupPathInnerLinks,

		groupPathLinksHolder,

		groupArrowHeads_0,

		groupNodes_0,
		groupGroupNodes_0,
		groupGroupNodes_1,

		groupMinimapNodes,

		groupColliders,

		groupEmpties,
		groupSphereEmpties,

		groupRenderSprites,

		groupPorts,
		groupPortSlots,

		//fore
		textLabels,
	};

	const std::unordered_map<Group, std::string> groupNames = {
		{groupBackgroundLayer, "groupBackgroundLayer" },
		{panelBackground, "panelBackground"},

		//action
		{ groupLinks_0,"groupLinks_0" },
		{groupGroupLinks_0, "groupGroupLinks_0"},
		{groupGroupLinks_1, "groupGroupLinks_1"},

		{groupPathLinks, "groupPathLinks"},

		{groupPathInnerLinks, "groupPathInnerLinks"},

		{groupPathLinksHolder, "groupPathLinksHolder"},

		{groupArrowHeads_0, "groupArrowHeads_0"},

		{ groupNodes_0,"groupNodes_0" },
		{ groupGroupNodes_0, "groupGroupNodes_0"},
		{ groupGroupNodes_1, "groupGroupNodes_1"},

		{ groupMinimapNodes,"groupMinimapNodes" },

		{ groupEmpties,"groupEmpties" },
		{ groupSphereEmpties,"groupSphereEmpties" },

		{ groupColliders,"groupColliders" },
		{ groupRenderSprites,"groupRenderSprites" },
		{ groupPorts,"groupPorts" },
		{ groupPortSlots,"groupPortSlots" },

		//fore
		{ textLabels,"textLabels" },
	};

	std::string getGroupName(Group mGroup) const;

	void scanComponentNames(const std::string& folderPath);

	void setComponentNames();
	template<typename T>
	std::vector<EntityID> getRevealedEntitiesInCameraCells();
	template<typename T>
	std::vector<EntityID> getEntitiesInCameraCells();
};