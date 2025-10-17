#pragma once
#include "../GECS/Core/GECSEntity.h"
#include "../GECS/Components.h"

#include "../AABB/AABB.h"

#include <vector>

#include <cmath>

constexpr int CELL_SIZE = 100;
constexpr int AXIS_CELLS = 80;
constexpr int DEPTH_AXIS_CELLS = 4;
constexpr int ROW_CELL_SIZE = AXIS_CELLS * CELL_SIZE;
constexpr int COLUMN_CELL_SIZE = AXIS_CELLS * CELL_SIZE;
constexpr int DEPTH_CELL_SIZE = DEPTH_AXIS_CELLS * CELL_SIZE;


struct GridLevelData {
	int numXCells, numYCells, numZCells = 0;
	int startX, endX, startY, endY, startZ, endZ = 0;
	float cameraMargin = 0.0f;
};

class Grid {
public:
	enum Level {
		Basic,
		Outer1,
		Outer2
	};

	std::vector<EntityID> visible_emptyEntities;
	std::vector<EntityID> visible_nodes;
	std::vector<EntityID> visible_links;

	Grid(int width, int height, int depth, int cellSize);
	~Grid();

	void setSize(int cellSize);
	void init(int width, int height, int depth, int cellSize);

	void createCells(Grid::Level size);

	void addLink(EntityID link, Grid::Level m_level);
	std::vector<Cell*> getLinkCells(EntityID link, Grid::Level m_level);
	void addLink(EntityID link, std::vector<Cell*> cell);

	void addEmpty(EntityID entity, Grid::Level m_level);

	void addNode(EntityID entity, Grid::Level m_level);
	void addEmpty(EntityID entity, Cell* cell);
	void addNode(EntityID entity, Cell* cell);

	Cell* getCell(int x, int y, int z, Grid::Level m_level);
	Cell* getCell(const Entity& position, Grid::Level m_level);
	std::vector<Cell*> getAdjacentCells(int x, int y, int z, Grid::Level m_level);
	std::vector<Cell*> getAdjacentCells(const Entity& entity, Grid::Level m_level);
	std::vector<Cell*>& getCells(Grid::Level m_level);
	int getCellSize();
	int getNumXCells();
	int getNumYCells();
	int getNumZCells();

	bool setIntersectedCameraCells(ICamera& camera);

	std::vector<Cell*> getIntersectedCameraCells(ICamera& camera);

	// loops through the intrecepted cells and just get the entities
	template <typename T>
	std::vector<T*> getRevealedEntitiesInCameraCells() {
		std::vector<EntityId> result;

		if constexpr (std::is_same_v<T, NodeEntity>) {
			for (auto& cell : _interceptedCells) {
				for (auto& entityId : cell->nodes) {
					auto& ent = getEntityFromId(entityId);
					if (!ent->isHidden()) {  // Check if the entity is visible
						result.push_back(entity);

						for (auto& port : entity->children) {
							if (port.second && !port.second->isHidden()) {
								visible_emptyEntities.push_back(static_cast<EmptyEntity*>(port.second));

								if (port.second->hasComponent<PortComponent>()) {
									for (auto& portSlots : port.second->children)
										visible_emptyEntities.push_back(static_cast<EmptyEntity*>(portSlots.second));
								}
							}
						}
					}
				}
			}
		}
		else if constexpr (std::is_same_v<T, EmptyEntity>) {
			for (auto& cell : _interceptedCells) {
				for (auto& entity : cell->emptyEntities) {
					if (!entity->isHidden()) {  // Check if the entity is visible
						result.push_back(entity);
					}
				}
			}
		}
		else if constexpr (std::is_same_v<T, LinkEntity>) {
			std::map<unsigned int, LinkEntity*> uniqueEntities;

			for (auto& cell : _interceptedCells) {
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
	std::vector<T*> getEntitiesInCameraCells() {
		std::vector<T*> result;

		if constexpr (std::is_same_v<T, NodeEntity>) {
			for (auto& cell : _interceptedCells) {
				result.insert(result.end(), cell->nodes.begin(), cell->nodes.end());
			}

			for (auto& cell : _interceptedCells) {
				for (auto& entity : cell->nodes) {
					if (!entity->isHidden()) {
						// Also include children(ports) if they exist
						for (auto& port : entity->children) {
							if (port.second && !port.second->isHidden()) {
								visible_emptyEntities.push_back(static_cast<EmptyEntity*>(port.second));

								if (port.second->hasComponent<PortComponent>()) {
									for (auto& portSlots : port.second->children)
										visible_emptyEntities.push_back(static_cast<EmptyEntity*>(portSlots.second));
								}
							}
						}
					}
				}
			}

		}
		else if constexpr (std::is_same_v<T, EmptyEntity>) {
			for (auto& cell : _interceptedCells) {
				result.insert(result.end(), cell->emptyEntities.begin(), cell->emptyEntities.end());
			}
		}
		else {
			static_assert(sizeof(T) == 0, "Unsupported entity type.");
		}
		return result;
	}

	std::vector<LinkEntity*> getLinksInCameraCells();


	bool gridLevelChanged();

	Level getGridLevel();
	void setGridLevel(Level newLevel);

	int getLevelCellScale();

	int getLevelCellScale(Level level);
private:
	std::vector<Cell*> _interceptedCells;

	std::vector<Cell*> _cells;
	std::vector<Cell*> _parentCells;
	std::vector<Cell*> _superParentCells;

	int _cellSize;

	int _width;
	int _height;
	int _depth;

	int _numXCells;
	int _numYCells;
	int _numZCells;

	// can change between different scenes/managers
	std::map<Level, GridLevelData> gridLevelsData;

	std::map<Level, int> gridLevels = {
		{Level::Basic,  1},
		{Level::Outer1, 2},
		{Level::Outer2, 4}
	};

	Level _level = Level::Basic;
	Level _lastLevel = Level::Basic;
};