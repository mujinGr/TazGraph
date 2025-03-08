#pragma once

#include "./CellEntity.h"

class LinkEntity;

class EmptyEntity : public CellEntity {	
protected:
	ComponentArray componentArray;//create 2 arrays, this is for the fast access
	ComponentBitSet componentBitSet;

	Entity* parent_entity = nullptr;
public:
	EmptyEntity(Manager& mManager) : CellEntity(mManager) {}
	
	
	template <typename T> bool hasComponent() const
	{
		return this && componentBitSet[GetComponentTypeID<T>()];
	}

	//! have addScript function
	template <typename T, typename... TArgs>
	T& addComponent(TArgs&&... mArgs)
	{
		T* c(new T(std::forward<TArgs>(mArgs)...));
		c->entity = this;
		std::unique_ptr<BaseComponent> uPtr{ c };
		components.emplace_back(std::move(uPtr));

		componentArray[GetComponentTypeID<T>()] = c;
		componentBitSet[GetComponentTypeID<T>()] = true;
		c->id = GetComponentTypeID<T>();

		c->init();
		return *c;
	}

	template<typename T> T& GetComponent() const
	{
		auto ptr(componentArray[GetComponentTypeID<T>()]);
		return *static_cast<T*>(ptr);
	}

	Entity* getParentEntity() {
		return parent_entity;
	}

	void setParentEntity(Entity* pEntity) {
		parent_entity = pEntity;
	}
};

class NodeEntity : public EmptyEntity {
private:
	NodeComponentArray node_componentArray;//create 2 arrays, this is for the fast access
	NodeComponentBitSet node_componentBitSet;

protected:
	std::vector<LinkEntity*> inLinks;
	std::vector<LinkEntity*> outLinks;
public:
	NodeEntity(Manager& mManager) : EmptyEntity(mManager) {

	}

	
	template <typename T> bool hasComponent() const
	{
		if constexpr (std::is_base_of_v<NodeComponent, T>) {
			return this && node_componentBitSet[GetNodeComponentTypeID<T>()];
		}
		else {
			return this && componentBitSet[GetComponentTypeID<T>()];
		}
		return false;
	}

	//! have addScript function
	template <typename T, typename... TArgs>
	T& addComponent(TArgs&&... mArgs)
	{
		T* c(new T(std::forward<TArgs>(mArgs)...));
		c->entity = this;
		std::unique_ptr<BaseComponent> uPtr{ c };
		components.emplace_back(std::move(uPtr));

		if constexpr (std::is_base_of_v<NodeComponent, T>) {
			node_componentArray[GetNodeComponentTypeID<T>()] = c;
			node_componentBitSet[GetNodeComponentTypeID<T>()] = true;
			c->id = GetNodeComponentTypeID<T>();
		}
		else {
			componentArray[GetComponentTypeID<T>()] = c;
			componentBitSet[GetComponentTypeID<T>()] = true;
			c->id = GetComponentTypeID<T>();
		}


		c->init();
		return *c;
	}

	template<typename T> T& GetComponent() const
	{
		if constexpr (std::is_base_of_v<NodeComponent, T>) {
			auto ptr(node_componentArray[GetNodeComponentTypeID<T>()]);
			return *static_cast<T*>(ptr);
		}
		else {
			auto ptr(componentArray[GetComponentTypeID<T>()]);
			return *static_cast<T*>(ptr);
		}
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
private:
	LinkComponentArray componentArray;//create 2 arrays, this is for the fast access
	LinkComponentBitSet componentBitSet;

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

	LinkEntity(Manager& mManager, NodeEntity* mfrom, NodeEntity* mto)
		: MultiCellEntity(mManager), from(mfrom), to(mto) {
	}

	LinkEntity(Manager& mManager, unsigned int mfromId, unsigned int mtoId)
		: MultiCellEntity(mManager), fromId(mfromId), toId(mtoId) {
	}

	template <typename T> bool hasComponent() const
	{
		return this && componentBitSet[GetLinkComponentTypeID<T>()];
	}

	template <typename T, typename... TArgs>
	T& addComponent(TArgs&&... mArgs)
	{
		T* c(new T(std::forward<TArgs>(mArgs)...));
		c->entity = this;
		std::unique_ptr<BaseComponent> uPtr{ c };
		components.emplace_back(std::move(uPtr));

		componentArray[GetLinkComponentTypeID<T>()] = c;
		componentBitSet[GetLinkComponentTypeID<T>()] = true;
		c->id = GetLinkComponentTypeID<T>();

		c->init();
		return *c;
	}

	template<typename T> T& GetComponent() const
	{
		auto ptr(componentArray[GetLinkComponentTypeID<T>()]);
		return *static_cast<T*>(ptr);
	}

	// instead of virtual functions you can instead do dynamic casting on derived classes to get the functions
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

	virtual void updateLinkPorts() {

	}

};