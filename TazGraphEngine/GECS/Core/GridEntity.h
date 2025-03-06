#pragma once

#include "GECS.h"
#include "../../Grid/Grid.h"

class CellEntity : public EmptyEntity_Base {

public:
	CellEntity(Manager& mManager) : EmptyEntity_Base(mManager) {

	}

	Cell* ownerCell = nullptr;

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

class GridLinkEntity : public LinkEntity_Base {
public:
	GridLinkEntity(Manager& mManager) : LinkEntity_Base(mManager) {
	}

	std::vector<Cell*> ownerCells = {};

	void removeFromCell() {
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

	void setOwnerCell(Cell* cell) {
		this->ownerCells.push_back(cell);
	}

	Cell* getOwnerCell() const { return ownerCells[0]; }

};