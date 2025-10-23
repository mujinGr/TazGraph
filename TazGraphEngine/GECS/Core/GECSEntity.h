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
			removeEntityFromCell();
			this->ownerCell = nullptr;
		}
	}

	void removeEntityFromCell() override {
		auto* cell = ownerCell;
		if (!cell) return;

		std::scoped_lock lock(cell->mtx); // automatically releases when leaving scope

		auto& entities = cell->emptyEntities;
		auto it = std::find(entities.begin(), entities.end(), id);
		if (it != entities.end())
			entities.erase(it);
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

	void removeEntityFromCell() override {
		auto* cell = ownerCell;
		if (!cell) return;

		std::scoped_lock lock(cell->mtx); // automatically releases when leaving scope

		auto& entities = cell->nodes;
		auto it = std::find(entities.begin(), entities.end(), id);
		if (it != entities.end())
			entities.erase(it);
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
	//! When Node_to_Node
	EntityID fromId = 0;
	EntityID toId = 0;

	NodeEntity* from = nullptr;
	NodeEntity* to = nullptr;

public:
	enum class ConnectionType {
		NODE_TO_NODE,
		PORT_TO_PORT,
		DIRECT_POSITIONS
	};
	ConnectionType type = ConnectionType::NODE_TO_NODE;


	//! When Port_to_Port
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
		removeEntityFromCell();
		ownerCells.clear();
	}

	void removeEntityFromCell() override {
		for (auto* cell : ownerCells) {
			if (!cell) continue;

			// Lock per-cell to prevent concurrent erase
			std::scoped_lock lock(cell->mtx);

			auto& links = cell->links;
			auto it = std::find(links.begin(), links.end(), id);
			if (it != links.end()) {
				links.erase(it);
			}
		}
	}

	NodeEntity* getFromNode() const {
		return from;
	}

	NodeEntity* getToNode() const {
		return to;
	}

	EntityID getFromPort() {
		return from->children[fromPort];
	}

	EntityID getToPort() {
		return to->children[toPort];
	}

	virtual void updateConnectedPorts() {}

	virtual void resetPorts() {}

	virtual void updateArrowHeads() {}

	virtual void addArrowHead() {}

	virtual void removeArrowHead() {}
};