#include "Grid.h"
#include "../GECS/Components.h"
#include <unordered_set>

Grid::Grid(int width, int height, int cellSize)
	: _width(width), _height(height), _cellSize(cellSize)
{
	_numXCells = ceil((float)_width / _cellSize);
	_numYCells = ceil((float)_height / _cellSize);

	createCells(Level::Basic);
	createCells(Level::Outer1);
	createCells(Level::Outer2);

}

Grid::~Grid()
{
}

void Grid::createCells(Grid::Level m_level) {
	int cellsGroupSize = m_level == Level::Basic ? 1 : (m_level == Level::Outer1 ? 2 : 4);
	
	int numXCells = ceil((float)_numXCells / cellsGroupSize);
	int numYCells = ceil((float)_numYCells / cellsGroupSize);
	
	if (m_level == Level::Basic) {
		_cells.resize(numYCells * numXCells);
	}
	else if (m_level == Level::Outer1) {
		_parentCells.resize(numYCells * numXCells);
	}
	else {
		_superParentCells.resize(numYCells * numXCells);
	}

	for (int py = -(numYCells / 2); py < (numYCells / 2); py++) {
		for (int px = -(numXCells / 2); px < (numXCells / 2); px++) {
			int parentIndex = (py + (numYCells / 2)) * numXCells + (px + (numXCells / 2));

			Cell& cell = (m_level == Level::Basic) ? _cells[parentIndex] :
				(m_level == Level::Outer1) ? _parentCells[parentIndex] :
					_superParentCells[parentIndex];

			cell.boundingBox.x = px * cellsGroupSize * _cellSize;
			cell.boundingBox.y = py * cellsGroupSize * _cellSize;
			cell.boundingBox.w = cellsGroupSize * _cellSize;
			cell.boundingBox.h = cellsGroupSize * _cellSize;

			if (px == numXCells / 2 - 1) {
				cell.boundingBox.w = _width - cell.boundingBox.x;
			}
			if (py == numYCells / 2 - 1) {
				cell.boundingBox.h = _height - cell.boundingBox.y;
			}

			if (m_level != Level::Outer2)
			{
				for (int cy = 0; cy < cellsGroupSize; cy++) {
					for (int cx = 0; cx < cellsGroupSize; cx++) {

						int childX = (px + (numXCells / 2)) * cellsGroupSize + cx;
						int childY = (py + (numYCells / 2)) * cellsGroupSize + cy;
						
						if (childX < numXCells && childY < numYCells) {
							int childIndex = childY * numXCells + childX;
							if (m_level == Level::Basic) {
								_cells[childIndex].parent = &cell;
							}
							else if (m_level == Level::Outer1) {
								_parentCells[childIndex].parent = &cell;
							}
						}
					}
				}
			}
		}
	}

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
void Grid::addNode(Entity* entity)
{
	Cell* cell = getCell(*entity);
	addNode(entity, cell);
}

void Grid::addNode(Entity* entity, Cell* cell)
{
	cell->nodes.push_back(entity);

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

std::vector<Cell*> Grid::getAdjacentCells(int x, int y, int radius = 1) {
	std::vector<Cell*> adjacentCells;

	int numCells = (2 * radius + 1) * (2 * radius + 1);
	adjacentCells.reserve(numCells);

	int cellX = (int)(x / _cellSize);
	int cellY = (int)(y / _cellSize);

	for (int offsetX = -radius; offsetX <= radius; offsetX++) {
		for (int offsetY = -radius; offsetY <= radius; offsetY++) {
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

std::vector<Cell*> Grid::getIntersectedCameraCells(ICamera& camera) {
	std::vector<Cell*> result;

	for (auto& cell : this->_cells) {
		if (checkCollision(camera.getCameraRect(), cell.boundingBox)) { // Assuming each cell has a bounding box
			result.push_back(&cell);
		}
	}
	return result;
}

std::vector<Entity*> Grid::getRevealedNodesInCameraCells(const std::vector<Cell*>& intercepted_cells) {
	std::vector<Entity*> result;

	for (auto& cell : intercepted_cells) {
		for (auto& entity : cell->nodes) {
			if (!entity->isHidden()) {  // Check if the entity is visible
				result.push_back(entity);
			}
		}
	}
	return result;
}

std::vector<Entity*> Grid::getNodesInCameraCells(const std::vector<Cell*>& intercepted_cells) {
	std::vector<Entity*> result;

	for (auto& cell : intercepted_cells) {
		result.insert(result.end(), cell->nodes.begin(), cell->nodes.end());
	}
	return result;
}

bool Grid::gridLevelChanged() {
	if (_level != _lastLevel) {
		_lastLevel = _level; // Update the last known grid level
		return true;
	}
	return false;
}


Grid::Level Grid::getGridLevel()
{
	return _level;
}

void Grid::setGridLevel(Level newLevel)
{
	_level = newLevel;
}

float Grid::getLevelScale(Level level) {
	return gridLevels[level];
}


std::vector<Entity*> Grid::getLinksInCameraCells(const std::vector<Cell*>& intercepted_cells) {
	std::map<unsigned int, Entity*> uniqueEntities;

	for (auto& cell : intercepted_cells) {
		for (auto& link : cell->links) {
			if (!link->isHidden()) {
				unsigned int linkId = link->getId();

				if (uniqueEntities.find(linkId) == uniqueEntities.end()) {
					uniqueEntities[linkId] = link;
				}
			}
		}
	}

	std::vector<Entity*> result;

	for (auto& entry : uniqueEntities) {
		result.push_back(entry.second);
	}
	return result;
}

bool Grid::hasCellsChanged(const std::vector<Cell*>& intercepted_cells) {
	if (intercepted_cells.size() != _lastInterceptedCells.size()) {
		return true; // Different sizes mean they definitely changed
	}

	// Check if all elements are the same
	for (size_t i = 0; i < intercepted_cells.size(); ++i) {
		if (intercepted_cells[i] != _lastInterceptedCells[i]) {
			return true; // Cells are not the same
		}
	}
	return false; // No changes found
}

void Grid::updateLastInterceptedCells(const std::vector<Cell*>& intercepted_cells) {
	_lastInterceptedCells = intercepted_cells;
}
