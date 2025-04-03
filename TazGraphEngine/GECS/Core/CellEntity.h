#pragma once

#include "GECS.h"

struct Cell {
	std::vector<EmptyEntity*> emptyEntities;
	std::vector<NodeEntity*> nodes;
	std::vector<LinkEntity*> links;

	glm::vec3 boundingBox_origin; // Starting point (minimum corner) of the cell
	glm::vec3 boundingBox_size;
	glm::vec3 boundingBox_center;

	Cell* parent = nullptr;
	std::vector<Cell*> children;

	template <typename T>
	std::vector<T*>& getEntityList() {
		if constexpr (std::is_same_v<T, NodeEntity>) {
			return nodes;
		}
		else if constexpr (std::is_same_v<T, EmptyEntity>) {
			return emptyEntities;
		}
		else if constexpr (std::is_same_v<T, LinkEntity>) {
			return links;
		}
		else {
			static_assert(sizeof(T) == 0, "Unsupported entity type.");
		}
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