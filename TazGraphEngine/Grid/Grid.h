#pragma once
#include "../GECS/GECS.h"

#include <vector>

#include <cmath>

struct Cell {
	std::vector<Entity*> entities;
};


class Grid {
	friend class Manager;
public:
	Grid(int width, int height, int cellSize);
	~Grid();

	void addEntity(Entity* entity);
	void addEntity(Entity* entity, Cell* cell);
	void removeEntity(Entity* entity);
	Cell* getCell(int x, int y);
	Cell* getCell(const Entity& position);
	std::vector<Cell*> getAdjacentCells(const Entity& entity);
	std::vector<Cell> getCells();
	int getCellSize();
	int getNumXCells();
	int getNumYCells();

private:
	std::vector<Cell> _cells;
	int _cellSize;
	int _width;
	int _height;
	int _numXCells;
	int _numYCells;
};