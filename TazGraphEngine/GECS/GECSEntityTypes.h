#pragma once

#include "GECSManager.h"

typedef class LinkEntity : public Entity {
private:

	unsigned int fromId = 0;
	unsigned int toId = 0;

	Entity* from = nullptr;
	Entity* to = nullptr;

public:

	LinkEntity(Manager& mManager) : Entity(mManager) {
		isLink = true;
	}

	LinkEntity(Manager& mManager, unsigned int mfromId, unsigned int mtoId) 
		: Entity(mManager),
		fromId(mfromId),
		toId(mtoId)
	{
		isLink = true;
		from = &mManager.getEntityFromId(fromId);
		to = &mManager.getEntityFromId(toId);
	}

	~LinkEntity() {

	}

	Entity* getFromNode() const override {
		return from;
	}

	Entity* getToNode() const override {
		return to;
	}
} Link;


typedef class NodeEntity : public Entity {
private:
	Entity* parent_entity = nullptr;

public:

	NodeEntity(Manager& mManager) : Entity(mManager) {

	}
	~NodeEntity() {

	}

	Entity* getParentEntity() override {
		return parent_entity;
	}

	void setParentEntity(Entity* pEntity) override {
		parent_entity = pEntity;
	}
} Node;