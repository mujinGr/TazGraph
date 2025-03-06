#pragma once
#include "../GECS/Core/GECS.h"
#include "../AABB/AABB.h"

#include <vector>

#include <cmath>

//class NodeEntity : Entity;

struct Cell {
	std::vector<Entity*> nodes;
	std::vector<Entity*> links;

	glm::vec3 boundingBox_origin; // Starting point (minimum corner) of the cell
	glm::vec3 boundingBox_size;

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

	Grid(int width, int height, int depth, int cellSize);
	~Grid();

	void createCells(Grid::Level size);

	void addLink(Entity* link, Grid::Level m_level);
	std::vector<Cell*> getLinkCells(const Entity& link, Grid::Level m_level);
	void addLink(Entity* link, Cell* cell);

	void addNode(Entity* entity, Grid::Level m_level);
	void addNode(Entity* entity, Cell* cell);

	Cell* getCell(int x, int y, int z, Grid::Level m_level);
	Cell* getCell(const Entity& position, Grid::Level m_level);
	std::vector<Cell*> getAdjacentCells(int x, int y, int z, Grid::Level m_level);
	std::vector<Cell*> getAdjacentCells(const Entity& entity, Grid::Level m_level);
	std::vector<Cell>& getCells(Grid::Level m_level);
	int getCellSize();
	int getNumXCells();
	int getNumYCells();

	bool setIntersectedCameraCells(ICamera& camera);

	std::vector<Cell*> getIntersectedCameraCells(ICamera& camera);

	std::vector<Entity*> getRevealedNodesInCameraCells();

	std::vector<Entity*> getLinksInCameraCells();

	std::vector<Entity*> getNodesInCameraCells();

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