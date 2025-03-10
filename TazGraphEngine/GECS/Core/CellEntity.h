#pragma once

#include "GECS.h"

struct Cell {
	std::vector<EmptyEntity*> emptyEntities;
	std::vector<NodeEntity*> nodes;
	std::vector<LinkEntity*> links;

	glm::vec3 boundingBox_origin; // Starting point (minimum corner) of the cell
	glm::vec3 boundingBox_size;

	Cell* parent = nullptr;
	std::vector<Cell*> children;

	template <typename T>
	std::vector<T*>& getEntityList();

	template <>
	std::vector<NodeEntity*>& getEntityList<NodeEntity>() {
		return nodes;
	}

	template <>
	std::vector<EmptyEntity*>& getEntityList<EmptyEntity>() {
		return emptyEntities;
	}

	template <>
	std::vector<LinkEntity*>& getEntityList<LinkEntity>() {
		return links;
	}
};


class CellEntity : public Entity {
public:
	Cell* ownerCell = nullptr;


	CellEntity(Manager& mManager) : Entity(mManager) {

	}

	void setOwnerCell(Cell* cell) {
		this->ownerCell = cell;
	}


	Cell* getOwnerCell() const { return ownerCell; }

};

class MultiCellEntity : public Entity {
public:
	std::vector<Cell*> ownerCells = {};

	MultiCellEntity(Manager& mManager) : Entity(mManager) {

	}

	void setOwnerCells(std::vector<Cell*> cells) {
		this->ownerCells = cells;
	}

	Cell* getOwnerCells() const { return ownerCells[0]; }

};