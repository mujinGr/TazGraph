#include "Grid.h"
#include "../GOS/Components.h"

Grid::Grid(int width, int height, int cellSize)
	: _width(width), _height(height), _cellSize(cellSize)
{
	_numXCells = ceil((float)_width / _cellSize);
	_numYCells = ceil((float)_height / _cellSize);

	//Allocate all the calls
	_cells.resize(_numYCells * _numXCells);
}

Grid::~Grid()
{
}

void Grid::addEntity(Entity* entity)
{
	Cell* cell = getCell(*entity);
	addEntity(entity, cell);
}

void Grid::addEntity(Entity* entity, Cell* cell)
{
	cell->entities.push_back(entity);

	entity->ownerCell = cell;
}

void Grid::removeEntity(Entity* entity) {
	entity->ownerCell->entities.erase(
		std::remove(entity->ownerCell->entities.begin(), entity->ownerCell->entities.end(),
			entity),
		entity->ownerCell->entities.end());
}


Cell* Grid::getCell(int x, int y)
{
	if (x < 0) x = 0;
	if (x >= _numXCells) x = _numXCells - 1;
	if (y < 0) y = 0;
	if (y >= _numYCells) y = _numYCells - 1;


	return &_cells[y * _numXCells + x];
}

Cell* Grid::getCell(const Entity& entity)
{
	auto pos = entity.GetComponent<TransformComponent>().getPosition();
	int cellX = (int)(pos.x / _cellSize);
	int cellY = (int)(pos.y / _cellSize);

	return getCell(cellX, cellY);
}

// Returns a vector of pointers to all adjacent cells (including diagonally adjacent cells)
std::vector<Cell*> Grid::getAdjacentCells(const Entity& entity) {
	std::vector<Cell*> adjacentCells;

	auto pos = entity.GetComponent<TransformComponent>().getPosition();
	int cellX = (int)(pos.x / _cellSize);
	int cellY = (int)(pos.y / _cellSize);

	for (int offsetX = -1; offsetX <= 1; offsetX++) {
		for (int offsetY = -1; offsetY <= 1; offsetY++) {
			if (offsetX == 0 && offsetY == 0) {
				adjacentCells.push_back(entity.ownerCell);
				continue;
			}
			int neighborX = cellX + offsetX;
			int neighborY = cellY + offsetY;

			// Check bounds and add the cell to the list
			if (neighborX >= 0 && neighborX < _numXCells &&
				neighborY >= 0 && neighborY < _numYCells) {
				adjacentCells.push_back(getCell(neighborX, neighborY));
			}
		}
	}
	return adjacentCells;
}

std::vector<Cell> Grid::getCells() {
	return _cells;
}
int Grid::getCellSize() {
	return _cellSize;
}
int Grid::getNumXCells() {
	return _numXCells;
}
int Grid::getNumYCells() {
	return _numYCells;
}