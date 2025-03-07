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

};

class CellEntity : public Entity {

public:
	Cell* ownerCell = nullptr;


	CellEntity(Manager& mManager) : Entity(mManager) {

	}


	void removeFromCell() {
		if (this->ownerCell) {
			removeEntity();
			this->ownerCell = nullptr;
		}
	}

	void removeEntity() {
		ownerCell->nodes.erase(
			std::remove(this->ownerCell->nodes.begin(), this->ownerCell->nodes.end(),
				this),
			this->ownerCell->nodes.end());
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


	void removeFromCells() {
		removeEntity();
		ownerCells.clear();
	}

	void removeEntity() {
		for (auto cell : ownerCells) {
			cell->links.erase(std::remove(cell->links.begin(), cell->links.end(),
				this),
				cell->links.end());
		}
	}

	void setOwnerCells(Cell* cell) {
		this->ownerCells.push_back(cell);
	}

	Cell* getOwnerCell() const { return ownerCells[0]; }

};