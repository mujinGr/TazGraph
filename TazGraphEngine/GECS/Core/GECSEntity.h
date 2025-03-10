#pragma once

#include "CellEntity.h"

class LinkEntity;

class EmptyEntity : public CellEntity {
protected:
	Entity* parent_entity = nullptr;
public:

	EmptyEntity(Manager& mManager) : CellEntity(mManager) {}

	void setComponentEntity(Component* c)  {
		std::cout << "adding component empty" << std::endl;
	}

	Entity* getParentEntity() override {
		return parent_entity;
	}

	void setParentEntity(Entity* pEntity) override {
		parent_entity = pEntity;
	}

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
protected:
	std::vector<Entity*> inLinks;
	std::vector<Entity*> outLinks;
public:

	NodeEntity(Manager& mManager) : EmptyEntity(mManager) {

	}
	void setComponentEntity(NodeComponent* c)  {
		std::cout << "adding component node" << std::endl;

	}

	void removeEntity() {
		ownerCell->nodes.erase(
			std::remove(this->ownerCell->nodes.begin(), this->ownerCell->nodes.end(),
				this),
			this->ownerCell->nodes.end());
	}

	void addInLink(Entity* link) {
		inLinks.push_back(link);
	}

	void addOutLink(Entity* link) {
		outLinks.push_back(link);
	}

	const std::vector<Entity*>& getInLinks() const {
		return inLinks;
	}

	const std::vector<Entity*>& getOutLinks() const {
		return outLinks;
	}

};


class LinkEntity : public MultiCellEntity {
protected:
	unsigned int fromId = 0;
	unsigned int toId = 0;



public:
	std::string fromPort;
	std::string toPort;

	LinkEntity(Manager& mManager) : MultiCellEntity(mManager) {
	}

	LinkEntity(Manager& mManager, unsigned int mfromId, unsigned int mtoId)
		: MultiCellEntity(mManager), fromId(mfromId), toId(mtoId) {
	}

	void setComponentEntity(LinkComponent* c) {
		std::cout << "adding component link" << std::endl;

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