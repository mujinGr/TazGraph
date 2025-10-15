#pragma once

#include "CellEntity.h"

class LinkEntity;

class EmptyEntity : public CellEntity {
protected:
	Entity* parent_entity = nullptr;
public:

	EmptyEntity(Manager& mManager) : CellEntity(mManager) {}

	void setComponentEntity(Component* c) override {
		c->entity = this;
	}

	Entity* getParentEntity() override {
		return parent_entity;
	}

	void setParentEntity(Entity* pEntity, const char* newID = "") override {
		parent_entity = pEntity;
		if ((newID != NULL) && (newID[0] == '\0')) {
			id = (parent_entity ? EntityIDUtils::toString(parent_entity->getId()) : "") + newID;
		}
		else {
			id = (parent_entity ? EntityIDUtils::toString(parent_entity->getId()) : "") + EntityIDUtils::toString(id);
		}
	}

	void removeFromCell() override {
		if (this->ownerCell) {
			removeEntity();
			this->ownerCell = nullptr;
		}
	}

	void removeEntity() override {
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

	void removeInLink(LinkEntity* link) {
		auto it = std::find(inLinks.begin(), inLinks.end(), link);
		if (it != inLinks.end()) {
			inLinks.erase(it);
		}
	}

	void removeOutLink(LinkEntity* link) {
		auto it = std::find(outLinks.begin(), outLinks.end(), link);
		if (it != outLinks.end()) {
			outLinks.erase(it);
		}
	}

	const std::vector<LinkEntity*>& getInLinks() const {
		return inLinks;
	}

	const std::vector<LinkEntity*>& getOutLinks() const {
		return outLinks;
	}

	virtual void addPorts() {}

	virtual void removePorts() {}
	virtual void removeSlots() {}

	virtual void updatePorts(float deltaTime) {}

};


class LinkEntity : public MultiCellEntity {
protected:
	EntityID fromId = 0;
	EntityID toId = 0;

	NodeEntity* from = nullptr;
	NodeEntity* to = nullptr;

public:
	EntityID fromPort = -1;
	EntityID toPort = -1;
	int fromSlotIndex = -1;
	int toSlotIndex = -1;

	LinkEntity(Manager& mManager) : MultiCellEntity(mManager) {

	}

	LinkEntity(Manager& mManager, EntityID mfromId, EntityID mtoId)
		: MultiCellEntity(mManager), fromId(mfromId), toId(mtoId) {

	}

	LinkEntity(Manager& mManager, NodeEntity* mfrom, NodeEntity* mto)
		: MultiCellEntity(mManager), from(mfrom), to(mto) {

	}

	LinkEntity(Manager& mManager,
		EntityID mfromId, EntityID mtoId,
		NodeEntity* mfrom, NodeEntity* mto)
		: MultiCellEntity(mManager),
		fromId(mfromId), toId(mtoId),
		from(mfrom), to(mto) {

	}

	LinkEntity(
		Manager& mManager,
		NodeEntity* mfrom, NodeEntity* mto,
		EntityID m_fromPort, EntityID m_toPort, int m_fromSlot, int m_toSlot)
		: MultiCellEntity(mManager), from(mfrom), to(mto)
	{
		fromPort = m_fromPort;
		toPort = m_toPort;
		fromSlotIndex = m_fromSlot;
		toSlotIndex = m_toSlot;
	}

	void setComponentEntity(LinkComponent* c) override {
		c->entity = this;
	}

	void removeFromCells() override {
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

	Entity* getFromPort() {
		return from->children[fromPort];
	}

	Entity* getToPort() {
		return to->children[toPort];
	}

	virtual void updateConnectedPorts() {}

	virtual void resetPorts() {}

	virtual void updateArrowHeads() {}

	virtual void addArrowHead() {}

	virtual void removeArrowHead() {}
};