#pragma once

#include "GECS.h"
#include "../../Grid/Grid.h"

#include "../../Threader/Threader.h"
#include <regex>
#include <filesystem>

namespace fs = std::filesystem;


class Manager
{
private:
	Threader* _threader = nullptr;
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

	std::vector<NodeEntity*> movedNodes;
	std::mutex movedNodesMutex;

	bool arrowheadsEnabled = false;
	bool last_arrowheadsEnabled = false;

	std::unordered_map<std::string, std::vector<std::string>> componentNames;

	std::unique_ptr<Grid> grid;

	Manager() {}

	~Manager() { _threader = nullptr;  }

	void setThreader(Threader& mthreader) {
		_threader = &mthreader;
	}

	void update(float deltaTime = 1.0f)
	{
		//! THREADER CHECK
		
		if (_threader && !_threader->t_queue.shuttingDown) {

			//! CELL UPDATE

			//_threader->parallel(visible_emptyEntities.size(), [&](int start, int end) {
			//	for (int i = start; i < end; i++) {
			//		if (visible_emptyEntities[i] && visible_emptyEntities[i]->isActive()) {
			//			visible_emptyEntities[i]->cellUpdate();
			//		}
			//	}
			//	});

			//_threader->parallel(visible_nodes.size(), [&](int start, int end) {
			//	for (int i = start; i < end; i++) {
			//		if (visible_nodes[i] && visible_nodes[i]->isActive()) {
			//			visible_nodes[i]->cellUpdate();
			//		}
			//	}
			//	});

			//! UPDATE LINK CELLS
			//? THIS MAY CAUSE ERRORS, IF REMOVE LINK FROM CELL AND OTHER LINK THAT HAS THAT CELL IN SEARCH
			//? WILL PUMP IN AN EMPTY ELEMENT OR THE SIZE WILL BE SMALLER FOR THAT LINK TO FIND ELEMENT
			for (auto& e : movedNodes) {
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
						link->updateLinkToPorts();
					}
				}
				});

			_threader->parallel(movedNodes.size(), [&](int start, int end) {
				for (int i = start; i < end; i++) {
					for (auto& link : movedNodes[i]->getOutLinks()) {
						link->updateLinkToPorts();
					}
				}
				});

			movedNodes.clear();

			//! UPDATE
			_threader->parallel(visible_emptyEntities.size(), [&](int start, int end) {
				for (int i = start; i < end; i++) {
					if (visible_emptyEntities[i] && visible_emptyEntities[i]->isActive()) {
						visible_emptyEntities[i]->update(deltaTime);
					}
				}
				});

			
			_threader->parallel(visible_nodes.size(), [&](int start, int end) {
				for (int i = start; i < end; i++) {
					if (visible_nodes[i] && visible_nodes[i]->isActive()) {
						visible_nodes[i]->update(deltaTime);
					}
				}

				});

			
			_threader->parallel(visible_links.size(), [&](int start, int end) {

				for (int i = start; i < end; i++) {
					if (visible_links[i] && visible_links[i]->isActive()) {
						visible_links[i]->update(deltaTime);
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
					link->updateLinkToPorts();
				}
			}

			for (auto& e : movedNodes) {
				for (auto& link : e->getOutLinks()) {
					link->updateLinkToPorts();
				}
			}

			movedNodes.clear();

			for (auto& e : visible_emptyEntities) {
				if (!e || !e->isActive()) continue;

				e->update(deltaTime);
			}

			if (arrowheadsEnabled) {
				for (auto& e : visible_nodes) {
					if (!e || !e->isActive()) continue;

					e->update(deltaTime);

				}
			}
			

			for (auto& e : visible_links) {
				if (!e || !e->isActive()) continue;

				e->update(deltaTime);
			}
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
	std::vector<T*>& getVisibleGroup(Group mGroup) {
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
	std::vector<T*>& getGroup(Group mGroup) {
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

		auto adjacentCells = grid->getAdjacentCells(*mainEntity, grid->getGridLevel());

		for (Cell* adjCell : adjacentCells) {
			for (auto& neighbor : adjCell->nodes) {
				if (neighbor->hasGroup(group) && (neighbor != mainEntity) ) { 
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

		groupEmpties,
		groupSphereEmpties,

		groupRenderSprites,
		
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

		{ groupEmpties,"groupEmpties" },
		{ groupSphereEmpties,"groupSphereEmpties" },

		{ groupColliders,"groupColliders" },
		{ groupRenderSprites,"groupRenderSprites" },


		//fore
		{ buttonLabels,"buttonLabels" },
	};

	std::string getGroupName(Group mGroup) const;

	void scanComponentNames(const std::string& folderPath);

	void setComponentNames();
};