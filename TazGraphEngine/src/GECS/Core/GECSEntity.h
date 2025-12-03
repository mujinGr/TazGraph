#pragma once

#include "CellEntity.h"

class LinkEntity;

class EmptyEntity : public CellEntity {
protected:
	Entity* parent_entity = nullptr; //! the object doesnt move, so we can have pointer
public:

	EmptyEntity(Manager& mManager) : CellEntity(mManager) {}

	void setComponentEntity(Component* c) override {
		c->entity = this;
	}

	Entity* getParentEntity() override {
		return parent_entity;
	}

	void setParentEntity(Entity* pEntity) override {
		parent_entity = pEntity;
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
		auto it = std::find(entities.begin(), entities.end(), getId());
		if (it != entities.end())
			entities.erase(it);
	}


};

class NodeEntity : public EmptyEntity {
protected:
	std::vector<EntityID> inLinks;
	std::vector<EntityID> outLinks;
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
		auto it = std::find(entities.begin(), entities.end(), getId());
		if (it != entities.end())
			entities.erase(it);
	}

	void addInLink(EntityID link) {
		inLinks.push_back(link);
	}

	void addOutLink(EntityID link) {
		outLinks.push_back(link);
	}

	void removeInLink(EntityID link) {
		auto it = std::find(inLinks.begin(), inLinks.end(), link);
		if (it != inLinks.end()) {
			inLinks.erase(it);
		}
	}

	void removeOutLink(EntityID link) {
		auto it = std::find(outLinks.begin(), outLinks.end(), link);
		if (it != outLinks.end()) {
			outLinks.erase(it);
		}
	}

	const std::vector<EntityID>& getInLinks() const {
		return inLinks;
	}

	const std::vector<EntityID>& getOutLinks() const {
		return outLinks;
	}

	virtual void addPorts() {}

	virtual void removePorts() {}
	virtual void removeSlots() {}

	virtual void updatePorts(float deltaTime) {}

};


class LinkEntity : public MultiCellEntity {
public:
	//! When Node_to_Node
	EntityID fromId = 0;
	EntityID toId = 0;

	enum class ConnectionType {
		NODE_TO_NODE,
		PORT_TO_PORT,
		DIRECT_POSITIONS
	};
	ConnectionType type = ConnectionType::NODE_TO_NODE;

	//! When DirectPosition

	glm::vec3 fromPos = glm::vec3(0);
	glm::vec3 toPos = glm::vec3(0);

	//! When Port_to_Port
	EntityID fromPort = -1;
	EntityID toPort = -1;
	int fromSlotIndex = -1;
	int toSlotIndex = -1;

	LinkEntity(Manager& mManager) : MultiCellEntity(mManager) {

	}

	LinkEntity(Manager& mManager, EntityID mfromId, EntityID mtoId)
		: MultiCellEntity(mManager), fromId(mfromId), toId(mtoId) {
		type = ConnectionType::NODE_TO_NODE;

	}

	LinkEntity(
		Manager& mManager,
		EntityID mfromId, EntityID mtoId,
		EntityID m_fromPort, EntityID m_toPort, int m_fromSlot, int m_toSlot)
		: MultiCellEntity(mManager), fromId(mfromId), toId(mtoId)
	{
		fromPort = m_fromPort;
		toPort = m_toPort;
		fromSlotIndex = m_fromSlot;
		toSlotIndex = m_toSlot;
		type = ConnectionType::PORT_TO_PORT;
	}

	LinkEntity(
		Manager& mManager,
		glm::vec3 mfrom, glm::vec3 mto
	)
		: MultiCellEntity(mManager), fromPos(mfrom), toPos(mto)
	{
		type = ConnectionType::DIRECT_POSITIONS;
	}

	void setComponentEntity(LinkComponent* c) override {
		c->entity = this;
	}

	void removeFromCells() override {
		removeEntityFromCell();
		ownerCells.clear();
	}

	void removeEntityFromCell() override {
		for (auto& cell : ownerCells) {
			if (!cell) continue;

			// Lock per-cell to prevent concurrent erase
			std::scoped_lock lock(cell->mtx);

			auto& links = cell->links;
			auto it = std::find(links.begin(), links.end(), getId());
			if (it != links.end()) {
				links.erase(it);
			}
		}
	}

	EntityID getFromNode() const {
		return fromId;
	}

	EntityID getToNode() const {
		return toId;
	}

	virtual void setConnectionType(ConnectionType setType) {}

	virtual void updateConnection() {}

	virtual void updatePortSlots() {}

	virtual void updateArrowHeads() {}

	virtual void addArrowHead() {}

	virtual void removeArrowHead() {}
};