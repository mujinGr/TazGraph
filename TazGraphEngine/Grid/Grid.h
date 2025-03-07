#pragma once
#include "../GECS/Core/GECSEntity.h"
#include "../AABB/AABB.h"

#include "../GECS/Components.h"

#include <vector>

#include <cmath>


class Grid {
public:
	enum CellEntityTypes {
		EMPTY,
		NODE,
		LINK
	};

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
	void addLink(LinkEntity* link, Cell* cell);

	void addNode(NodeEntity* entity, Grid::Level m_level);
	void addNode(NodeEntity* entity, Cell* cell);

	Cell* getCell(int x, int y, int z, Grid::Level m_level);
	Cell* getCell(const Entity& position, Grid::Level m_level);
	std::vector<Cell*> getAdjacentCells(int x, int y, int z, Grid::Level m_level);
	std::vector<Cell*> getAdjacentCells(const NodeEntity& entity, Grid::Level m_level);
	std::vector<Cell>& getCells(Grid::Level m_level);
	int getCellSize();
	int getNumXCells();
	int getNumYCells();

	bool setIntersectedCameraCells(ICamera& camera);

	std::vector<Cell*> getIntersectedCameraCells(ICamera& camera);


	template <typename T>
	std::vector<T*> getEntitiesInCameraCells();

	template <>
	std::vector<EmptyEntity*> getEntitiesInCameraCells();
	template <>
	std::vector<NodeEntity*> getEntitiesInCameraCells();
	template <>
	std::vector<LinkEntity*> getEntitiesInCameraCells();

	template <typename T>
	std::vector<T*> getRevealedEntitiesInCameraCells();
	
	template <>
	std::vector<EmptyEntity*> getRevealedEntitiesInCameraCells();
	template <>
	std::vector<NodeEntity*> getRevealedEntitiesInCameraCells();
	template <>
	std::vector<LinkEntity*> getRevealedEntitiesInCameraCells();


	bool gridLevelChanged();

	Level getGridLevel();
	void setGridLevel(Level newLevel);

	float getLevelScale(Level level);
	float getLevelCellScale(Level level);
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
	std::map<Level, std::pair<float, int>> gridLevels = {
		{Level::Basic,  {0.02f, 1}},
		{Level::Outer1, {0.01f, 2} },
		{Level::Outer2, {0.0f, 4}}
	};

	Level _level = Level::Basic;
	Level _lastLevel = Level::Basic;
};