#pragma once

#include "CellEntity.h"

class LinkEntity;

class EmptyEntity : public CellEntity {
protected:
	Entity* parent_entity = nullptr;
public:

	EmptyEntity(Manager& mManager) : CellEntity(mManager) {}

	void setComponentEntity(Component* c) override {
		std::cout << "adding component empty" << std::endl;

		c->entity = this;
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

	void removeEntity() override{
		ownerCell->emptyEntities.erase(
			std::remove(this->ownerCell->emptyEntities.begin(), this->ownerCell->emptyEntities.end(),
				this),
			this->ownerCell->emptyEntities.end());
	}


};

class NodeEntity : public EmptyEntity {
protected:
	std::vector<LinkEntity*> inLinks;
	std::vector<LinkEntity*> outLinks;
public:

	NodeEntity(Manager& mManager) : EmptyEntity(mManager) {
		nodeComponentArray.emplace();
		nodeComponentBitSet.emplace();
	}
	void setComponentEntity(NodeComponent* c) override {
		std::cout << "adding component node" << std::endl;

		c->entity = this;

	}

	void removeEntity() override {
		ownerCell->nodes.erase(
			std::remove(this->ownerCell->nodes.begin(), this->ownerCell->nodes.end(),
				this),
			this->ownerCell->nodes.end());
	}

	void addInLink(LinkEntity* link) {
		inLinks.push_back(link);
	}

	void addOutLink(LinkEntity* link) {
		outLinks.push_back(link);
	}

	const std::vector<LinkEntity*>& getInLinks() const {
		return inLinks;
	}

	const std::vector<LinkEntity*>& getOutLinks() const {
		return outLinks;
	}

	virtual void addPorts() {}

	virtual void removePorts() {}

};


class LinkEntity : public MultiCellEntity {
protected:
	unsigned int fromId = 0;
	unsigned int toId = 0;

	NodeEntity* from = nullptr;
	NodeEntity* to = nullptr;

public:
	std::string fromPort;
	std::string toPort;

	LinkEntity(Manager& mManager) : MultiCellEntity(mManager) {
	}

	LinkEntity(Manager& mManager, unsigned int mfromId, unsigned int mtoId)
		: MultiCellEntity(mManager), fromId(mfromId), toId(mtoId) {
	}

	LinkEntity(Manager& mManager, NodeEntity* mfrom, NodeEntity* mto)
		: MultiCellEntity(mManager), from(mfrom), to(mto) {
	}

	void setComponentEntity(LinkComponent* c) override {
		std::cout << "adding component link" << std::endl;

		c->entity = this;

	}

	void removeFromCells() {
		removeEntity();
		ownerCells.clear();
	}

	void removeEntity() override {
		for (auto cell : ownerCells) {
			cell->links.erase(std::remove(cell->links.begin(), cell->links.end(),
				this),
				cell->links.end());
		}
	}

	NodeEntity* getFromNode() const {
		return from;
	}

	NodeEntity* getToNode() const {
		return to;
	}

	EmptyEntity* getFromPort() {
		return from->children[fromPort];
	}

	EmptyEntity* getToPort() {
		return to->children[toPort];
	}

	virtual void updateLinkToPorts() {}

	virtual void updateLinkToNodes() {}

	virtual void addArrowHead() {}

	virtual void removeArrowHead() {}
};