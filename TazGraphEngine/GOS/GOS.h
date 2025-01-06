#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <bitset>
#include <array>

#include <SDL2/SDL.h>
#include "../SpriteBatch/SpriteBatch.h"
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
using Layer = int;

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
constexpr std::size_t maxGroups = 64;

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
	virtual void draw(SpriteBatch&  batch, TazGraphEngine::Window& window) {}

	virtual SDL_Rect getRect() 
	{
		SDL_Rect a;
		a.x = a.y = a.w = a.h = 0;
		return a;
	}

	virtual bool getHasGridAbove()
	{
		return false;
	}

	virtual ~Component() {}
};

class Entity
{
private:
	unsigned int id;

	Manager& manager;
	bool active = true;
	ComponentArray componentArray;//create 2 arrays, this is for the fast access
	ComponentBitSet componentBitSet;
	GroupBitSet groupBitSet;

	Entity* parent_entity = nullptr;

public:
	bool paused = false;

	void setId(unsigned int m_id) { id = m_id; }
	unsigned int getId() { return id; }

	bool checkCollision(const SDL_Rect recA, const SDL_FRect recB) {
		if (recA.x > recB.x + recB.w + CULLING_OFFSET || recA.x + recA.w < recB.x - CULLING_OFFSET ||
			recA.y > recB.y + recB.h + CULLING_OFFSET || recA.y + recA.h < recB.y - CULLING_OFFSET) {
			return false; // no collision
		}
		return true;
	}
	Cell* ownerCell = nullptr;
	std::vector<std::unique_ptr<Component>> components; //create 2 arrays, this is for the concurrent access
	Entity(Manager& mManager) : manager(mManager) {}

	void update(float deltaTime)
	{
		
		for (auto& c : components) {
			c->update(deltaTime); // start from which was added first
			if (c->id == 0 && paused)
			{
				break;
			}
		}
	}
	void updateFully(float deltaTime)
	{
		for (auto& c : components) {
			c->update(deltaTime); // start from which was added first
		}
	}
	void draw(SpriteBatch&  batch, TazGraphEngine::Window& window) 
	{
		for (auto& c : components) { 
			c->draw(batch, window); 
		}
	}
	bool isActive() { return active; }
	void destroy() { active = false; } // destroy happens relative to the group referencing

	bool hasGroup(Group mGroup)
	{
		return groupBitSet[mGroup];
	}

	void addGroup(Group mGroup);
	void delGroup(Group mGroup)
	{
		groupBitSet[mGroup] = false;
	}

	float getLayerValue(Layer layer_indx) {
		return (float) layer_indx / 100.0f;
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

	Entity* getParentEntity() {
		return parent_entity;
	}

	void setParentEntity(Entity* pEntity) {
		parent_entity = pEntity;
	}

};

