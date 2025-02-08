#pragma once
#include "../GECS/GECS.h"
#include "../AABB/AABB.h"

#include <vector>

#include <cmath>

struct Cell {
	std::vector<Entity*> nodes;
	std::vector<Entity*> links;

	SDL_FRect boundingBox;

	Cell* parent = nullptr;
	std::vector<Cell*> children;

};


class Grid {
public:
	enum Level {
		Basic,
		Outer1,
		Outer2
	};

	Grid(int width, int height, int cellSize);
	~Grid();

	void createCells(Grid::Level size);

	void addLink(Entity* link, Grid::Level m_level);
	std::vector<Cell*> getLinkCells(const Entity& link, Grid::Level m_level);
	void addLink(Entity* link, Cell* cell);

	void addNode(Entity* entity, Grid::Level m_level);
	void addNode(Entity* entity, Cell* cell);

	Cell* getCell(int x, int y, Grid::Level m_level);
	Cell* getCell(const Entity& position, Grid::Level m_level);
	std::vector<Cell*> getAdjacentCells(int x, int y, Grid::Level m_level);
	std::vector<Cell*> getAdjacentCells(const Entity& entity, Grid::Level m_level);
	std::vector<Cell>& getCells(Grid::Level m_level);
	int getCellSize();
	int getNumXCells();
	int getNumYCells();

	std::vector<Cell*> getIntersectedCameraCells(ICamera& camera, Grid::Level m_level);

	std::vector<Entity*> getRevealedNodesInCameraCells(const std::vector<Cell*>& intercepted_cells);

	std::vector<Entity*> getLinksInCameraCells(const std::vector<Cell*>& intercepted_cells);

	bool hasCellsChanged(const std::vector<Cell*>& intercepted_cells);

	void updateLastInterceptedCells(const std::vector<Cell*>& intercepted_cells);

	std::vector<Entity*> getNodesInCameraCells(const std::vector<Cell*>& intercepted_cells);

	bool gridLevelChanged();

	Level getGridLevel();
	void setGridLevel(Level newLevel);

	float getLevelScale(Level level);
	float getLevelCellScale(Level level);
private:
	std::vector<Cell*> _lastInterceptedCells;
	
	std::vector<Cell> _cells;
	std::vector<Cell> _parentCells;
	std::vector<Cell> _superParentCells;

	int _cellSize;
	int _width;
	int _height;
	int _numXCells;
	int _numYCells;

	// can change between different scenes/managers
	std::map<Level, std::pair<float, int>> gridLevels = {
		{Level::Basic,  {0.5f, 1}},
		{Level::Outer1, {0.1f, 2} },
		{Level::Outer2, {0.0f, 4}}
	};

	Level _level;
	Level _lastLevel;
};