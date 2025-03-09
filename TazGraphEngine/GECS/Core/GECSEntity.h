#pragma once

#include "CellEntity.h"

class LinkEntity;

class EmptyEntity : public CellEntity {
public:

	EmptyEntity(Manager& mManager) : CellEntity(mManager) {}

	void removeFromCell() {
		if (this->ownerCell) {
			removeEntity();
			this->ownerCell = nullptr;
		}
	}

	void removeEntity() {
		ownerCell->emptyEntities.erase(
			std::remove(this->ownerCell->emptyEntities.begin(), this->ownerCell->emptyEntities.end(),
				this),
			this->ownerCell->emptyEntities.end());
	}


};

class NodeEntity : public EmptyEntity {
public:

	NodeEntity(Manager& mManager) : EmptyEntity(mManager) {

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



};


class LinkEntity : public MultiCellEntity {
public:

	LinkEntity(Manager& mManager) : MultiCellEntity(mManager) {
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



};