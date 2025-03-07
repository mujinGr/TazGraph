#pragma once

#include "./CellEntity.h"

class LinkEntity;

class EmptyEntity : public CellEntity {
protected:
	Entity* parent_entity = nullptr;
public:
	EmptyEntity(Manager& mManager) : CellEntity(mManager) {}

	Entity* getParentEntity() {
		return parent_entity;
	}

	void setParentEntity(Entity* pEntity) {
		parent_entity = pEntity;
	}
};

class NodeEntity : public EmptyEntity {
protected:
	std::vector<LinkEntity*> inLinks;
	std::vector<LinkEntity*> outLinks;
public:
	NodeEntity(Manager& mManager) : EmptyEntity(mManager) {

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

	// instead of virtual functions you can instead do dynamic casting on derived classes to get the functions
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

};