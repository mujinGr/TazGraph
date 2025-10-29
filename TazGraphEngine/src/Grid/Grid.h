#pragma once

#include "../../pch.h"

#include "../GECS/Core/GECSEntity.h"
#include "../GECS/Components.h"

#include "../AABB/AABB.h"


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



	Grid(int width, int height, int depth, int cellSize);
	~Grid();

	void setSize(int cellSize);
	void init(int width, int height, int depth, int cellSize);

	void createCells(Grid::Level size);

	void addLink(LinkEntity* link, Grid::Level m_level);
	std::vector<Cell*> getLinkCells(LinkEntity* link, Grid::Level m_level);
	void addLink(LinkEntity* link, std::vector<Cell*> cell);

	void addEmpty(EmptyEntity* entity, Grid::Level m_level);

	void addNode(NodeEntity* entity, Grid::Level m_level);
	void addEmpty(EmptyEntity* entity, Cell* cell);
	void addNode(NodeEntity* entity, Cell* cell);

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

	std::vector<EntityID> getLinksInCameraCells();


	bool gridLevelChanged();

	Level getGridLevel();
	void setGridLevel(Level newLevel);

	int getLevelCellScale();

	int getLevelCellScale(Level level);
	std::vector<Cell*> interceptedCells;
private:

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