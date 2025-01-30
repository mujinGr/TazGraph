#pragma once
#include "../GECS/GECS.h"
#include "../AABB/AABB.h"

#include <vector>

#include <cmath>

struct Cell {
	std::vector<Entity*> nodes;
	std::vector<Entity*> links;

	SDL_FRect boundingBox;
};


class Grid {
	friend class Manager;
public:
	Grid(int width, int height, int cellSize);
	~Grid();

	void addLink(Entity* link);
	std::vector<Cell*> getLinkCells(const Entity& link);
	void addLink(Entity* link, Cell* cell);

	void addNode(Entity* entity);
	void addNode(Entity* entity, Cell* cell);

	Cell* getCell(int x, int y);
	Cell* getCell(const Entity& position);
	std::vector<Cell*> getAdjacentCells(int x, int y);
	std::vector<Cell*> getAdjacentCells(const Entity& entity);
	std::vector<Cell> getCells();
	int getCellSize();
	int getNumXCells();
	int getNumYCells();

	std::vector<Cell*> getIntercectedCameraCells(ICamera& camera);

	std::vector<Entity*> getRevealedNodesInCameraCells(const std::vector<Cell*>& intercepted_cells);

	std::vector<Entity*> getLinksInCameraCells(const std::vector<Cell*>& intercepted_cells);

	std::vector<Entity*> getNodesInCameraCells(const std::vector<Cell*>& intercepted_cells);

	float getGroupingZoomLevel();
private:
	std::vector<Cell> _cells;
	int _cellSize;
	int _width;
	int _height;
	int _numXCells;
	int _numYCells;
	float _groupingZoomLevel = 0.5f;
};