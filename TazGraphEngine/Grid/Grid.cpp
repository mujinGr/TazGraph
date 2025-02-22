#include "Grid.h"
#include "../GECS/Components.h"
#include "../GECS/UtilComponents.h"

#include <unordered_set>

Grid::Grid(int width, int height, int depth, int cellSize)
	: _width(width), _height(height), _depth(depth), _cellSize(cellSize)
{
	_numXCells = ceil((float)_width / _cellSize);
	_numYCells = ceil((float)_height / _cellSize);
	_numZCells = ceil((float)_depth / _cellSize);


	createCells(Level::Basic);
	createCells(Level::Outer1);
	createCells(Level::Outer2);

}

Grid::~Grid()
{
}

void Grid::createCells(Grid::Level m_level) {
	int cellsGroupSize = gridLevels[m_level].second;
	
	int numXCells = ceil((float)_numXCells / cellsGroupSize);
	int numYCells = ceil((float)_numYCells / cellsGroupSize);
	int numZCells = ceil((float)_numZCells / cellsGroupSize);

	
	std::vector<Cell>& currentCells =	(m_level == Level::Basic) ? _cells :
		(m_level == Level::Outer1) ? _parentCells :
		_superParentCells;

	std::vector<Cell> emptyCells;
	std::vector<Cell>& childCells =		(m_level == Level::Outer1) ? _cells :
		(m_level == Level::Outer2) ? _parentCells :
		emptyCells;

	currentCells.resize(numYCells * numXCells * numZCells);

	float startX = -(numXCells / 2.0f); // add one in order to take floor of division
	float endX = (numXCells) / 2.0f;
	float startY = -(numYCells / 2.0f);
	float endY = (numYCells ) / 2.0f;
	float startZ = -(numZCells / 2.0f);
	float endZ = (numZCells) / 2.0f;

	for (float pz = startZ; pz < endZ; pz++) {
		for (float py = startY; py < endY; py++) {
			for (float px = startX; px < endX; px++) {
				int parentIndex = ((pz - startZ) * numYCells * numXCells) +
					((py - startY) * numXCells) +
					(px - startX); // we dont want negative numbers thats why add the -startXY

				Cell& cell = currentCells[parentIndex];

				cell.boundingBox_origin.x = px * cellsGroupSize * _cellSize;
				cell.boundingBox_origin.y = py * cellsGroupSize * _cellSize;
				cell.boundingBox_origin.z = pz * cellsGroupSize * _cellSize;

				cell.boundingBox_size.x = cellsGroupSize * _cellSize;
				cell.boundingBox_size.y = cellsGroupSize * _cellSize;
				cell.boundingBox_size.z = cellsGroupSize * _cellSize;

				if (px == endX - 1.0f) {
					cell.boundingBox_size.x = (_width / 2.0f) - cell.boundingBox_origin.x;
				}
				if (py == endY - 1.0f) {
					cell.boundingBox_size.y = (_height / 2.0f) - cell.boundingBox_origin.y;
				}
				if (pz == endZ - 1.0f) { // Edge case for Z
					cell.boundingBox_size.z = (_depth / 2.0f) - cell.boundingBox_origin.z;
				}

				if (!childCells.empty())
				{
					int groupedCells = sqrt(childCells.size() / currentCells.size());
					for (int cz = 0; cz < groupedCells; cz++) {
						for (int cy = 0; cy < groupedCells; cy++) {
							for (int cx = 0; cx < groupedCells; cx++) {

								int childX = (px + endY) * groupedCells + cx;
								int childY = (py + endY) * groupedCells + cy;
								int childZ = (pz + endZ) * groupedCells + cz;

								int childIndex = (childZ * numYCells * numXCells * groupedCells) +
									(childY * numXCells * groupedCells) +
									(childX);

								if (childIndex < childCells.size()) {
									cell.children.push_back(&childCells[childIndex]);
								}
							}
						}
					}
				}
			}
		}
	}

}

// adding link to grid
void Grid::addLink(Entity* link, Grid::Level m_level)
{
	std::vector<Cell*> cells = getLinkCells(*link, m_level);
	for (auto& cell : cells) {
		addLink(link, cell);
	}
}

std::vector<Cell*> Grid::getLinkCells(const Entity& link, Grid::Level m_level) {
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
		Cell* currentCell = getCell((int)(std::floor(x0 / _cellSize)), (int)(std::floor(y0 / _cellSize)), m_level );
		if (uniqueCells.insert(currentCell).second) { 
			intersectedCells.push_back(currentCell);
		}
		x0 += xIncrement;
		y0 += yIncrement;
	}
	Cell* currentCell = getCell((int)(std::floor(x1 / _cellSize)), (int)(std::floor(y1 / _cellSize)), m_level);

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
void Grid::addNode(Entity* entity, Grid::Level m_level)
{
	Cell* cell = getCell(*entity, m_level);
	addNode(entity, cell);
}

void Grid::addNode(Entity* entity, Cell* cell)
{
	cell->nodes.push_back(entity);

	entity->setOwnerCell(cell);
}


Cell* Grid::getCell(int x, int y, Grid::Level m_level)
{
	int cellsGroupSize = gridLevels[m_level].second;

	int numXCells = ceil((float)_numXCells / cellsGroupSize);
	int numYCells = ceil((float)_numYCells / cellsGroupSize);

	float startX	= ceil(-((numXCells) / 2)) ; // add one in order to take floor of division
	float endX		= ceil((numXCells) / 2) - 1;
	float startY	= ceil(-((numYCells) / 2));
	float endY		= ceil((numYCells) / 2) - 1;


	if (x < startX) x = startX;
	if (x >= endX) x = endX;
	if (y < startY) y = startY;
	if (y >= endY) y = endY;

	size_t index = (y - startY) * numXCells + (x - startX);
	return (m_level == Level::Basic) ? &_cells[index] :
		(m_level == Level::Outer1 ? &_parentCells[index] :
			&_superParentCells[index]);
}

Cell* Grid::getCell(const Entity& entity, Grid::Level m_level)
{
	auto pos = entity.GetComponent<TransformComponent>().getCenterTransform();
	if (m_level == Grid::Level::Outer2) {
		pos -= (_cellSize * gridLevels[m_level].second) / 2;
	}
	int cellX = (int)(std::floor((pos.x) / (_cellSize * gridLevels[m_level].second) ));
	int cellY = (int)(std::floor((pos.y) / (_cellSize * gridLevels[m_level].second) ));

	return getCell(cellX, cellY, m_level);
}

std::vector<Cell*> Grid::getAdjacentCells(int x, int y, Grid::Level m_level) {
	std::vector<Cell*> adjacentCells;

	int cellsGroupSize = gridLevels[m_level].second;

	int numXCells = ceil((float)_numXCells / cellsGroupSize);
	int numYCells = ceil((float)_numYCells / cellsGroupSize);

	float startX = ceil(-((numXCells) / 2)); // add one in order to take floor of division
	float endX = ceil((numXCells) / 2) - 1;
	float startY = ceil(-((numYCells) / 2));
	float endY = ceil((numYCells) / 2) - 1;

	adjacentCells.reserve(9);

	for (int offsetX = -1; offsetX <= 1; offsetX++) {
		for (int offsetY = -1; offsetY <= 1; offsetY++) {
			if (offsetX == 0 && offsetY == 0) {
				adjacentCells.push_back(getCell(x, y, _level));
				continue;
			}
			int neighborX = x + offsetX;
			int neighborY = y + offsetY;

			// Check bounds and add the cell to the list
			if (neighborX >= startX && neighborX < endX &&
				neighborY >= startY && neighborY < endY) {
				adjacentCells.push_back(getCell(neighborX, neighborY, _level));
			}
		}
	}
	return adjacentCells;
}

// Returns a vector of pointers to all adjacent cells (including diagonally adjacent cells)
std::vector<Cell*> Grid::getAdjacentCells(const Entity& entity, Grid::Level m_level) {
	std::vector<Cell*> adjacentCells;

	auto pos = entity.GetComponent<TransformComponent>().getPosition();

	if (m_level == Grid::Level::Outer2) {
		pos -= (_cellSize * gridLevels[m_level].second) / 2;
	}

	int cellX = (int)(std::floor((pos.x) / (_cellSize * gridLevels[m_level].second)));
	int cellY = (int)(std::floor((pos.y) / (_cellSize * gridLevels[m_level].second)));

	return getAdjacentCells(cellX, cellY, m_level);
}

std::vector<Cell>& Grid::getCells(Grid::Level m_level) {
	if(m_level == Grid::Level::Basic)
		return _cells;
	else if (m_level == Grid::Level::Outer1)
		return _parentCells;
	return _superParentCells;
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

bool Grid::setIntersectedCameraCells(ICamera& camera) {
	bool intersectedCellsChanged = false;
	std::vector<Cell*> newInterceptedCells;

	for (auto& cell : getCells(_level)) {
		glm::vec4 cellCenter(
			cell.boundingBox_origin.x + cell.boundingBox_size.x / 2.0f, cell.boundingBox_origin.y + cell.boundingBox_size.y / 2.0f,
			0.0f, 1.0f);
		if (camera.isPointInCameraView(cellCenter)) {
			newInterceptedCells.push_back(&cell);
		}
	}

	if (newInterceptedCells != _interceptedCells) {
		_interceptedCells = std::move(newInterceptedCells);
		intersectedCellsChanged = true;
	}

	return intersectedCellsChanged;
}

std::vector<Cell*> Grid::getIntersectedCameraCells(ICamera& camera) {
	return _interceptedCells;
}

std::vector<Entity*> Grid::getRevealedNodesInCameraCells() {
	std::vector<Entity*> result;

	for (auto& cell : _interceptedCells) {
		for (auto& entity : cell->nodes) {
			if (!entity->isHidden()) {  // Check if the entity is visible
				result.push_back(entity);
			}
		}
	}
	return result;
}

std::vector<Entity*> Grid::getNodesInCameraCells() {
	std::vector<Entity*> result;

	for (auto& cell : _interceptedCells) {
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
	return gridLevels[level].first;
}

float Grid::getLevelCellScale(Level level) {
	return gridLevels[level].second;
}

std::vector<Entity*> Grid::getLinksInCameraCells() {
	std::map<unsigned int, Entity*> uniqueEntities;

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

	std::vector<Entity*> result;

	for (auto& entry : uniqueEntities) {
		result.push_back(entry.second);
	}
	return result;
}
