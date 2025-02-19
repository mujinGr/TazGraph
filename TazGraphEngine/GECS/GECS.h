#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <bitset>
#include <array>
#include <unordered_map>

#include <SDL2/SDL.h>
#include "../Renderers/PlaneModelRenderer/PlaneModelRenderer.h"
#include "../Renderers/LineRenderer/LineRenderer.h"
#include "../Renderers/PlaneColorRenderer/PlaneColorRenderer.h"
#include "../Camera2.5D/CameraManager.h"
#include "../Window/Window.h"

#define CULLING_OFFSET 100

class Component;
class Entity;
class Manager;
class Window;
struct Cell;

using ComponentID = std::size_t;
using Group = std::size_t;
using Layer = std::size_t;

inline ComponentID getNewComponentTypeID()
{
	static ComponentID lastID = 0u;
	return lastID++;
}

template <typename T> inline ComponentID GetComponentTypeID() noexcept
{
	static ComponentID typeID = getNewComponentTypeID(); // typeID is unique for each function type T and only initialized once.
	return typeID;
}

constexpr std::size_t maxComponents = 32;
constexpr std::size_t maxGroups = 16;

using ComponentBitSet = std::bitset<maxComponents>;
using GroupBitSet = std::bitset<maxGroups>;

using ComponentArray = std::array<Component*, maxComponents>;

class Component
{
public:
	Entity* entity;

	ComponentID id = 0u;

	virtual void init(){}
	virtual void update(float deltaTime) {}
	virtual void draw(PlaneModelRenderer&  batch, TazGraphEngine::Window& window) {}
	virtual void draw(LineRenderer& batch, TazGraphEngine::Window& window) {}
	virtual void draw(PlaneColorRenderer& batch, TazGraphEngine::Window& window) {}

	virtual ~Component() {}
};

class Entity
{
private:
	unsigned int id;

	bool active = true; // false if about to delete
	bool hidden = false; // true if not do updates
	ComponentArray componentArray;//create 2 arrays, this is for the fast access
	ComponentBitSet componentBitSet;
	GroupBitSet groupBitSet;

protected:
	Manager& manager;
public:
	std::unordered_map<std::string,Entity*> children;

	void setId(unsigned int m_id) { id = m_id; }
	unsigned int getId() { return id; }

	void hide() {
		hidden = true;
	}

	void reveal() {
		hidden = false;
	}

	bool isHidden() {
		return hidden;
	}

	std::vector<std::unique_ptr<Component>> components; //create 2 arrays, this is for the concurrent access
	Entity(Manager& mManager) : manager(mManager) {}
	virtual ~Entity() {}

	virtual void update(float deltaTime)
	{
		
		for (auto& c : components) {
			c->update(deltaTime); // start from which was added first
		}
	}

	virtual void cellUpdate() {};

	virtual void removeFromCell() {};

	virtual void setOwnerCell(Cell* cell) {};

	virtual Cell* getOwnerCell() const { return nullptr; };

	void draw(PlaneModelRenderer& batch, TazGraphEngine::Window& window) 
	{
		for (auto& c : components) { 
			c->draw(batch, window); 
		}
	}
	void draw(LineRenderer& batch, TazGraphEngine::Window& window)
	{
		for (auto& c : components) {
			c->draw(batch, window);
		}
	}
	void draw(PlaneColorRenderer& batch, TazGraphEngine::Window& window)
	{
		for (auto& c : components) {
			c->draw(batch, window);
		}
	}
	bool isActive() { return active; }
	virtual void destroy() { active = false;
	} // destroy happens relative to the group referencing

	bool hasGroup(Group mGroup)
	{
		return groupBitSet[mGroup];
	}

	void addGroup(Group mGroup);
	void delGroup(Group mGroup)
	{
		groupBitSet[mGroup] = false;
	}

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
		std::unique_ptr<Component> uPtr{ c };
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

	// for when wanting to add new entities from components
	Manager* getManager() {
		return &manager;
	}

	virtual void addMessage(std::string mMessage) {}

	virtual const std::vector<Entity*>& getInLinks() const { return {}; }

	virtual const std::vector<Entity*>& getOutLinks() const { return {}; }

	// instead of virtual functions you can instead do dynamic casting on derived classes to get the functions
	virtual Entity* getFromNode() const {
		return nullptr;
	}

	virtual Entity* getToNode() const {
		return nullptr;
	}

	virtual Entity* getFromPort() {
		return nullptr;
	}

	virtual Entity* getToPort() {
		return nullptr;
	}

	virtual void updateLinkPorts() {}

	virtual Entity* getParentEntity() {
		return nullptr;
	}

	virtual void setParentEntity(Entity* pEntity) {}

	virtual void imgui_print() {}

	virtual void imgui_display() {}
};

