#include "Grid.h"
#include "../GECS/Components.h"

Grid::Grid(int width, int height, int cellSize)
	: _width(width), _height(height), _cellSize(cellSize)
{
	_numXCells = ceil((float)_width / _cellSize);
	_numYCells = ceil((float)_height / _cellSize);

	//Allocate all the calls
	_cells.resize(_numYCells * _numXCells);

	for (int y = -(_numYCells / 2); y < (_numYCells / 2); y++) {
		for (int x = -(_numXCells / 2); x < (_numXCells / 2); x++) {
			int index = (y + (_numYCells / 2)) * _numXCells + (x+ (_numXCells / 2));
			_cells[index].boundingBox.x = x * _cellSize;
			_cells[index].boundingBox.y = y * _cellSize;
			_cells[index].boundingBox.w = _cellSize;
			_cells[index].boundingBox.h = _cellSize;

			if (x == _numXCells - 1 && _width % _cellSize != 0) {
				_cells[index].boundingBox.w = _width - (x * _cellSize);
			}
			if (y == _numYCells - 1 && _height % _cellSize != 0) {
				_cells[index].boundingBox.h = _height - (y * _cellSize);
			}
		}
	}
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
	if (x < -(_numXCells/2)) x = -(_numXCells / 2);
	if (x >= (_numXCells/2)) x = (_numXCells / 2) - 1;
	if (y < -(_numYCells / 2)) y = -(_numYCells / 2);
	if (y >= (_numYCells / 2)) y = (_numYCells / 2) - 1;


	return &_cells[(y + (_numYCells / 2)) * _numXCells + (x + (_numXCells / 2))];
}

Cell* Grid::getCell(const Entity& entity)
{
	auto pos = entity.GetComponent<TransformComponent>().getPosition();
	int cellX = (int)(std::floor(pos.x / _cellSize));
	int cellY = (int)(std::floor(pos.y / _cellSize));

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

std::vector<Entity*> Grid::getEntitiesInCameraCells(ICamera& camera) {
	std::vector<Entity*> result;

	for (auto& cell : this->_cells) {
		if (checkCollision(camera.getCameraRect(), cell.boundingBox)) { // Assuming each cell has a bounding box
			result.insert(result.end(), cell.entities.begin(), cell.entities.end());
		}
	}
	return result;
}
