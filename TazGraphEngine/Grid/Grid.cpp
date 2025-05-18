#include "./Grid.h"

#include <unordered_set>

Grid::Grid(int width, int height, int depth, int cellSize)
{
	init(width, height, depth, cellSize);
}

Grid::~Grid()
{
}

void Grid::setCellSize(int cellSize) {
	init(_width, _height, _depth, cellSize);
}

void Grid::init(int width, int height, int depth, int cellSize)
{
	_width = width, _height = height, _depth = depth, _cellSize = cellSize;

	_numXCells = ceil((float)_width / _cellSize);
	_numYCells = ceil((float)_height / _cellSize);
	_numZCells = ceil((float)_depth / _cellSize);


	createCells(Level::Basic);
	gridLevelsData[Level::Basic].cameraMargin = 0.2f;
	createCells(Level::Outer1);
	gridLevelsData[Level::Outer1].cameraMargin = 0.3f;
	createCells(Level::Outer2);
	gridLevelsData[Level::Outer2].cameraMargin = 0.4f;

}

void Grid::createCells(Grid::Level m_level) {
	int cellsGroupSize = gridLevels[m_level];
	
	// push Grid Data
	GridLevelData data;
	data.numXCells = ceil((float)_numXCells / cellsGroupSize);
	data.numYCells = ceil((float)_numYCells / cellsGroupSize);
	data.numZCells = ceil((float)_numZCells / cellsGroupSize);

	data.startX =	ceil(-(data.numXCells / 2.0f)); // add one in order to take floor of division
	data.endX	=	ceil((data.numXCells) / 2.0f) - 1;
	data.startY =	ceil(-(data.numYCells / 2.0f));
	data.endY	=	ceil((data.numYCells) / 2.0f) - 1;
	data.startZ =	ceil(-(data.numZCells / 2.0f));
	data.endZ	=	ceil((data.numZCells) / 2.0f) - 1;

	gridLevelsData[m_level] = data;

	std::vector<Cell>& currentCells =	(m_level == Level::Basic) ? _cells :
		(m_level == Level::Outer1) ? _parentCells :
		_superParentCells;

	std::vector<Cell> emptyCells;
	std::vector<Cell>& childCells =		(m_level == Level::Outer1) ? _cells :
		(m_level == Level::Outer2) ? _parentCells :
		emptyCells;

	currentCells.resize(gridLevelsData[m_level].numYCells * gridLevelsData[m_level].numXCells * gridLevelsData[m_level].numZCells);


	for (int pz = gridLevelsData[m_level].startZ; pz <= gridLevelsData[m_level].endZ; pz++) {
		for (int py = gridLevelsData[m_level].startY; py <= gridLevelsData[m_level].endY; py++) {
			for (int px = gridLevelsData[m_level].startX; px <= gridLevelsData[m_level].endX; px++) {
				int parentIndex = 
					((pz - gridLevelsData[m_level].startZ) * gridLevelsData[m_level].numYCells * gridLevelsData[m_level].numXCells) +
					((py - gridLevelsData[m_level].startY) * gridLevelsData[m_level].numXCells) +
					(px - gridLevelsData[m_level].startX); // we dont want negative numbers thats why add the -startXY

				Cell& cell = currentCells[parentIndex];

				cell.boundingBox_origin.x = px * cellsGroupSize * _cellSize;
				cell.boundingBox_origin.y = py * cellsGroupSize * _cellSize;
				if(gridLevelsData[m_level].endZ != gridLevelsData[m_level].startZ)
					cell.boundingBox_origin.z = pz * cellsGroupSize * _cellSize;
				else
					cell.boundingBox_origin.z = (pz - 0.5f) * cellsGroupSize * _cellSize;

				cell.boundingBox_size.x = cellsGroupSize * _cellSize;
				cell.boundingBox_size.y = cellsGroupSize * _cellSize;
				cell.boundingBox_size.z = cellsGroupSize * _cellSize;

				cell.boundingBox_center.x = cell.boundingBox_origin.x + cell.boundingBox_size.x / 2.0f;
				cell.boundingBox_center.y = cell.boundingBox_origin.y + cell.boundingBox_size.y / 2.0f;
				cell.boundingBox_center.z = cell.boundingBox_origin.z + cell.boundingBox_size.z / 2.0f;

				if (px == gridLevelsData[m_level].endX) {
					cell.boundingBox_size.x = (_width / 2.0f) - cell.boundingBox_origin.x;
				}
				if (py == gridLevelsData[m_level].endY) {
					cell.boundingBox_size.y = (_height / 2.0f) - cell.boundingBox_origin.y;
				}
				if (pz == gridLevelsData[m_level].endZ && gridLevelsData[m_level].endZ != gridLevelsData[m_level].startZ) { // Edge case for Z
					cell.boundingBox_size.z = (_depth / 2.0f) - cell.boundingBox_origin.z;
				}

				if (!childCells.empty())
				{
					int groupedCells = sqrt(childCells.size() / currentCells.size());
					for (int cz = 0; cz < groupedCells; cz++) {
						for (int cy = 0; cy < groupedCells; cy++) {
							for (int cx = 0; cx < groupedCells; cx++) {

								int childX = (px - gridLevelsData[m_level].startX) * groupedCells + cx;
								int childY = (py - gridLevelsData[m_level].startY) * groupedCells + cy;
								int childZ = (pz - gridLevelsData[m_level].startZ) * groupedCells + cz;

								int childIndex = (childZ * _numYCells * _numXCells) +
									(childY * _numXCells) +
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
void Grid::addLink(LinkEntity* link, Grid::Level m_level)
{
	std::vector<Cell*> cells = getLinkCells(*link, m_level);
	
	addLink(link, cells);
}

std::vector<Cell*> Grid::getLinkCells(const LinkEntity& link, Grid::Level m_level) {
	std::vector<Cell*> intersectedCells;
	std::unordered_set<Cell*> uniqueCells;

	float x0 = link.getFromNode()->GetComponent<TransformComponent>().getCenterTransform().x;
	float y0 = link.getFromNode()->GetComponent<TransformComponent>().getCenterTransform().y;
	float z0 = link.getFromNode()->GetComponent<TransformComponent>().getPosition().z;

	float x1 = link.getToNode()->GetComponent<TransformComponent>().getCenterTransform().x;
	float y1 = link.getToNode()->GetComponent<TransformComponent>().getCenterTransform().y;
	float z1 = link.getToNode()->GetComponent<TransformComponent>().getPosition().z;

	float dx = x1 - x0;
	float dy = y1 - y0;
	float dz = z1 - z0;


	float distance = sqrt(dx * dx + dy * dy + dz * dz);
	float stepSize = 10;

	int steps = static_cast<int>(distance / stepSize);
	float xIncrement = dx / steps;
	float yIncrement = dy / steps;
	float zIncrement = dz / steps;

	for (int i = 0; i <= steps; i++) {
		Cell* currentCell = getCell(
			(int)(std::floor(x0 /  (_cellSize * gridLevels[m_level]))),
			(int)(std::floor(y0 / (_cellSize * gridLevels[m_level]))),
			(int)(std::floor(z0 / (_cellSize * gridLevels[m_level]))),
			m_level );
		if (uniqueCells.insert(currentCell).second) { 
			intersectedCells.push_back(currentCell);
		}
		x0 += xIncrement;
		y0 += yIncrement;
		z0 += zIncrement;
	}
	Cell* currentCell = getCell(
		(int)(std::floor(x1 / (_cellSize * gridLevels[m_level]))),
		(int)(std::floor(y1 / (_cellSize * gridLevels[m_level]))),
		(int)(std::floor(z1 / (_cellSize * gridLevels[m_level]))),
		m_level);

	if (uniqueCells.insert(currentCell).second) { 
		intersectedCells.push_back(currentCell);
	}

	return intersectedCells;
}

void Grid::addLink(LinkEntity* link, std::vector<Cell*> cells)
{
	for (auto& cell : cells) {
		cell->links.push_back(link);
	}

	link->setOwnerCells(cells);
}

// adding node to grid
void Grid::addEmpty(EmptyEntity* entity, Grid::Level m_level)
{
	Cell* cell = getCell(*entity, m_level);
	addEmpty(entity, cell);
}

void Grid::addNode(NodeEntity* entity, Grid::Level m_level)
{
	Cell* cell = getCell(*entity, m_level);
	addNode(entity, cell);
}

void Grid::addEmpty(EmptyEntity* entity, Cell* cell)
{
	cell->emptyEntities.push_back(entity);

	entity->setOwnerCell(cell);
}

void Grid::addNode(NodeEntity* entity, Cell* cell)
{
	cell->nodes.push_back(entity);

	entity->setOwnerCell(cell);
}


Cell* Grid::getCell(int x, int y, int z, Grid::Level m_level)
{
	if (x < gridLevelsData[m_level].startX) x = gridLevelsData[m_level].startX;
	if (x >= gridLevelsData[m_level].endX) x = gridLevelsData[m_level].endX;
	if (y < gridLevelsData[m_level].startY) y = gridLevelsData[m_level].startY;
	if (y >= gridLevelsData[m_level].endY) y = gridLevelsData[m_level].endY;
	if (z < gridLevelsData[m_level].startZ) z = gridLevelsData[m_level].startZ;
	if (z >= gridLevelsData[m_level].endZ) z = gridLevelsData[m_level].endZ;

	size_t index = (z - gridLevelsData[m_level].startZ) * gridLevelsData[m_level].numXCells * gridLevelsData[m_level].numYCells +
		(y - gridLevelsData[m_level].startY) * gridLevelsData[m_level].numXCells +
		(x - gridLevelsData[m_level].startX);
	return (m_level == Level::Basic) ? &_cells[index] :
		(m_level == Level::Outer1 ? &_parentCells[index] :
			&_superParentCells[index]);
}

Cell* Grid::getCell(const Entity& entity, Grid::Level m_level)
{
	auto pos = entity.GetComponent<TransformComponent>().getCenterTransform();

	int cellX = (int)(std::floor((pos.x) / (_cellSize * gridLevels[m_level]) ));
	int cellY = (int)(std::floor((pos.y) / (_cellSize * gridLevels[m_level]) ));
	int cellZ = (int)(std::floor((pos.z) / (_cellSize * gridLevels[m_level])));

	return getCell(cellX, cellY, cellZ, m_level);
}

std::vector<Cell*> Grid::getAdjacentCells(int x, int y, int z, Grid::Level m_level) {
	std::vector<Cell*> adjacentCells;

	adjacentCells.reserve(9);

	for (int offsetX = -1; offsetX <= 1; offsetX++) {
		for (int offsetY = -1; offsetY <= 1; offsetY++) {
			for (int offsetZ = -1; offsetZ <= 1; offsetZ++) {

				if (offsetX == 0 && offsetY == 0 && offsetZ == 0) {
					adjacentCells.push_back(getCell(x, y, z, _level));
					continue;
				}
				int neighborX = x + offsetX;
				int neighborY = y + offsetY;
				int neighborZ = z + offsetZ;

				// Check bounds and add the cell to the list
				if (neighborX >= gridLevelsData[m_level].startX && neighborX <= gridLevelsData[m_level].endX &&
					neighborY >= gridLevelsData[m_level].startY && neighborY <= gridLevelsData[m_level].endY &&
					neighborZ >= gridLevelsData[m_level].startZ && neighborZ <= gridLevelsData[m_level].endZ) {
					adjacentCells.push_back(getCell(neighborX, neighborY, neighborZ, _level));
				}
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
		pos -= (_cellSize * gridLevels[m_level]) / 2;
	}

	int cellX = (int)(std::floor((pos.x) / (_cellSize * gridLevels[m_level])));
	int cellY = (int)(std::floor((pos.y) / (_cellSize * gridLevels[m_level])));
	int cellZ = (int)(std::floor((pos.z) / (_cellSize * gridLevels[m_level])));

	return getAdjacentCells(cellX, cellY, cellZ, m_level);
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

int Grid::getNumZCells(){
	return _numZCells;
}

bool Grid::setIntersectedCameraCells(ICamera& camera) {
	bool intersectedCellsChanged = false;
	std::vector<Cell*> newInterceptedCells;

	for (auto& cell : getCells(_level)) {
		glm::vec4 cellCenter(
			cell.boundingBox_center,
			1.0f);
		if (camera.isPointInCameraView(cellCenter, gridLevelsData[_level].cameraMargin)) {
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

int Grid::getLevelCellScale() {
	return gridLevels[_level];
}

int Grid::getLevelCellScale(Level level) {
	return gridLevels[level];
}

std::vector<LinkEntity*> Grid::getLinksInCameraCells() {
	std::unordered_map<unsigned int, LinkEntity*> uniqueEntities;

	for (auto& cell : _interceptedCells) {
		for (auto& link : cell->links) {
			unsigned int linkId = link->getId();

			if (uniqueEntities.find(linkId) == uniqueEntities.end()) {
				uniqueEntities[linkId] = link;
			}
		}
	}

	std::vector<LinkEntity*> result;

	for (auto& entry : uniqueEntities) {
		result.push_back(entry.second);
	}
	return result;
}
