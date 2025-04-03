#pragma once
#include "../GECS/Core/GECSEntity.h"
#include "../GECS/Components.h"

#include "../AABB/AABB.h"

#include <vector>

#include <cmath>

struct GridLevelData {
	float numXCells, numYCells, numZCells;
	float startX, endX, startY, endY, startZ, endZ;
};

class Grid {
public:
	enum Level {
		Basic,
		Outer1,
		Outer2
	};

	Grid(int width, int height, int depth, int cellSize);
	~Grid();

	void createCells(Grid::Level size);

	void addLink(LinkEntity* link, Grid::Level m_level);
	std::vector<Cell*> getLinkCells(const LinkEntity& link, Grid::Level m_level);
	void addLink(LinkEntity* link, std::vector<Cell*> cell);

	void addEmpty(EmptyEntity* entity, Grid::Level m_level);

	void addNode(NodeEntity* entity, Grid::Level m_level);
	void addEmpty(EmptyEntity* entity, Cell* cell);
	void addNode(NodeEntity* entity, Cell* cell);

	Cell* getCell(int x, int y, int z, Grid::Level m_level);
	Cell* getCell(const Entity& position, Grid::Level m_level);
	std::vector<Cell*> getAdjacentCells(int x, int y, int z, Grid::Level m_level);
	std::vector<Cell*> getAdjacentCells(const Entity& entity, Grid::Level m_level);
	std::vector<Cell>& getCells(Grid::Level m_level);
	int getCellSize();
	int getNumXCells();
	int getNumYCells();
	int getNumZCells();

	bool setIntersectedCameraCells(ICamera& camera);

	std::vector<Cell*> getIntersectedCameraCells(ICamera& camera);

	template <typename T>
	std::vector<T*> getRevealedEntitiesInCameraCells() {
		std::vector<T*> result;

		if constexpr (std::is_same_v<T, NodeEntity>) {
			for (auto& cell : _interceptedCells) {
				for (auto& entity : cell->nodes) {
					if (!entity->isHidden()) {  // Check if the entity is visible
						result.push_back(entity);
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

	template <typename T>
	std::vector<T*> getEntitiesInCameraCells() {
		std::vector<T*> result;

		if constexpr (std::is_same_v<T, NodeEntity>) {
			for (auto& cell : _interceptedCells) {
				result.insert(result.end(), cell->nodes.begin(), cell->nodes.end());
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

	int getLevelCellScale(Level level);
private:
	std::vector<Cell*> _interceptedCells;
	
	std::vector<Cell> _cells;
	std::vector<Cell> _parentCells;
	std::vector<Cell> _superParentCells;

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