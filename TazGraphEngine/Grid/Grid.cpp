#include "Grid.h"
#include "../GECS/Components.h"
#include <unordered_set>

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

// adding link to grid
void Grid::addLink(Entity* link)
{
	std::vector<Cell*> cells = getLinkCells(*link);
	for (auto& cell : cells) {
		addLink(link, cell);
	}
}

std::vector<Cell*> Grid::getLinkCells(const Entity& link) {
	std::vector<Cell*> intersectedCells;
	std::unordered_set<Cell*> uniqueCells;

	float x0 = link.getFromNode()->GetComponent<TransformComponent>().getCenterTransform().x;
	float y0 = link.getFromNode()->GetComponent<TransformComponent>().getCenterTransform().y;
	float x1 = link.getToNode()->GetComponent<TransformComponent>().getCenterTransform().x;
	float y1 = link.getToNode()->GetComponent<TransformComponent>().getCenterTransform().y;
	
	float dx = x1 - x0;
	float dy = y1 - y0;

	float distance = sqrt(dx * dx + dy * dy);
	float stepSize = 10;

	int steps = static_cast<int>(distance / stepSize);
	float xIncrement = dx / steps;
	float yIncrement = dy / steps;

	for (int i = 0; i <= steps; i++) {
		Cell* currentCell = getCell((int)(std::floor(x0 / _cellSize)), (int)(std::floor(y0 / _cellSize)) );
		if (uniqueCells.insert(currentCell).second) { 
			intersectedCells.push_back(currentCell);
		}
		x0 += xIncrement;
		y0 += yIncrement;
	}
	Cell* currentCell = getCell((int)(std::floor(x1 / _cellSize)), (int)(std::floor(y1 / _cellSize)));

	if (uniqueCells.insert(currentCell).second) { 
		intersectedCells.push_back(currentCell);
	}

	return intersectedCells;
}

void Grid::addLink(Entity* link, Cell* cell)
{
	cell->links.push_back(link);

	link->setOwnerCell(cell);
}

// adding node to grid
void Grid::addEntity(Entity* entity)
{
	Cell* cell = getCell(*entity);
	addEntity(entity, cell);
}

void Grid::addEntity(Entity* entity, Cell* cell)
{
	cell->entities.push_back(entity);

	entity->setOwnerCell(cell);
}


Cell* Grid::getCell(int x, int y)
{
	if (x < -(_numXCells/2)) x = -(_numXCells / 2);
	if (x >= (_numXCells/2)) x = (_numXCells / 2) - 1;
	if (y < -(_numYCells / 2)) y = -(_numYCells / 2);
	if (y >= (_numYCells / 2)) y = (_numYCells / 2) - 1;

	size_t index = (y + (_numYCells / 2)) * _numXCells + (x + (_numXCells / 2));
	return &_cells[index];
}

Cell* Grid::getCell(const Entity& entity)
{
	auto pos = entity.GetComponent<TransformComponent>().getCenterTransform();
	int cellX = (int)(std::floor(pos.x / _cellSize));
	int cellY = (int)(std::floor(pos.y / _cellSize));

	return getCell(cellX, cellY);
}

std::vector<Cell*> Grid::getAdjacentCells(int x, int y) {
	std::vector<Cell*> adjacentCells;

	int cellX = (int)(x / _cellSize);
	int cellY = (int)(y / _cellSize);

	for (int offsetX = -1; offsetX <= 1; offsetX++) {
		for (int offsetY = -1; offsetY <= 1; offsetY++) {
			if (offsetX == 0 && offsetY == 0) {
				adjacentCells.push_back(getCell(cellX, cellY));
				continue;
			}
			int neighborX = cellX + offsetX;
			int neighborY = cellY + offsetY;

			// Check bounds and add the cell to the list
			if (neighborX >= -_numXCells/2 && neighborX < _numXCells/2 &&
				neighborY >= -_numYCells/2 && neighborY < _numYCells/2) {
				adjacentCells.push_back(getCell(neighborX, neighborY));
			}
		}
	}
	return adjacentCells;
}

// Returns a vector of pointers to all adjacent cells (including diagonally adjacent cells)
std::vector<Cell*> Grid::getAdjacentCells(const Entity& entity) {
	std::vector<Cell*> adjacentCells;

	auto pos = entity.GetComponent<TransformComponent>().getPosition();

	return getAdjacentCells(pos.x, pos.y);
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

std::vector<Cell*> Grid::getIntercectedCameraCells(ICamera& camera) {
	std::vector<Cell*> result;

	for (auto& cell : this->_cells) {
		if (checkCollision(camera.getCameraRect(), cell.boundingBox)) { // Assuming each cell has a bounding box
			result.push_back(&cell);
		}
	}
	return result;
}

std::vector<Entity*> Grid::getEntitiesInCameraCells(const std::vector<Cell*>& intercepted_cells) {
	std::vector<Entity*> result;

	for (auto& cell : intercepted_cells) {
		result.insert(result.end(), cell->entities.begin(), cell->entities.end());
	}
	return result;
}

std::vector<Entity*> Grid::getLinksInCameraCells(const std::vector<Cell*>& intercepted_cells) {
	std::map<unsigned int, Entity*> uniqueEntities;

	for (auto& cell : intercepted_cells) {
		for (auto& link : cell->links) {
			unsigned int linkId = link->getId();

			if (uniqueEntities.find(linkId) == uniqueEntities.end()) {
				uniqueEntities[linkId] = link;
			}
		}
	}

	std::vector<Entity*> result;

	for (auto& entry : uniqueEntities) {
		result.push_back(entry.second);
	}
	return result;
}
