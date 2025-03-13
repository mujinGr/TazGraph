#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <bitset>
#include <array>
#include <unordered_map>

#include <SDL2/SDL.h>
#include "../../Renderers/PlaneModelRenderer/PlaneModelRenderer.h"
#include "../../Renderers/LineRenderer/LineRenderer.h"
#include "../../Renderers/PlaneColorRenderer/PlaneColorRenderer.h"
#include "../../Camera2.5D/CameraManager.h"
#include "../../Window/Window.h"

#define CULLING_OFFSET 100

class BaseComponent;
class Entity;
class EmptyEntity;
class NodeEntity;
class LinkEntity;

class Manager;
class Window;
struct Cell;

using ComponentID = std::size_t;
using Group = std::size_t;

using layer = std::size_t;

namespace Layer {
	enum layerIndexes : std::size_t
	{
		action,
		menubackground
	};
}

const std::unordered_map<layer, float> layerNames = {
		{Layer::action, 0.0f},
		{Layer::menubackground, -100.0f}

};

inline float getLayerDepth(layer mLayer) {
	return layerNames.at(mLayer);
}


inline ComponentID getNewComponentTypeID()
{
	static ComponentID lastID = 0u;
	return lastID++;
}

inline ComponentID getNewLinkComponentTypeID()
{
	static ComponentID lastID_linkC = 0u;
	return lastID_linkC++;
}


template <typename T> inline ComponentID GetComponentTypeID() noexcept
{
	static ComponentID typeID = getNewComponentTypeID(); // typeID is unique for each function type T and only initialized once.
	return typeID;
}

template <typename T> inline ComponentID GetLinkComponentTypeID() noexcept
{
	static ComponentID typeID = getNewLinkComponentTypeID(); // typeID is unique for each function type T and only initialized once.
	return typeID;
}

constexpr std::size_t maxComponents = 32;
constexpr std::size_t maxGroups = 16;

using ComponentBitSet = std::bitset<maxComponents>;
using GroupBitSet = std::bitset<maxGroups>;

using ComponentArray = std::array<BaseComponent*, maxComponents>;

class BaseComponent
{
public:

	ComponentID id = 0u;

	virtual void init() {}
	virtual void update(float deltaTime) {}
	virtual void draw(PlaneModelRenderer& batch, TazGraphEngine::Window& window) {}
	virtual void draw(LineRenderer& batch, TazGraphEngine::Window& window) {}
	virtual void draw(PlaneColorRenderer& batch, TazGraphEngine::Window& window) {}

	virtual ~BaseComponent() {}
};

class Component : public BaseComponent {
public:
	Entity* entity;
};

class NodeComponent : public BaseComponent {
public:
	NodeEntity* entity;
};

class LinkComponent : public BaseComponent {
public:
	LinkEntity* entity;
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

	std::vector<std::unique_ptr<BaseComponent>> components; //create 2 arrays, this is for the concurrent access
	
	Entity(Manager& mManager) : manager(mManager) {}
	virtual ~Entity() {}

	virtual void update(float deltaTime)
	{
		
		for (auto& c : components) {
			c->update(deltaTime); // start from which was added first
		}
	}

	virtual void cellUpdate() {};

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

	virtual void addGroup(Group mGroup);
	void delGroup(Group mGroup)
	{
		groupBitSet[mGroup] = false;
	}

	template <typename T> bool hasComponent() const
	{
		if constexpr (std::is_base_of_v<LinkComponent, T>) {
			return this && componentBitSet[GetLinkComponentTypeID<T>()];
		}
		return this && componentBitSet[GetComponentTypeID<T>()];
	}
	//! have addScript function
	template <typename T, typename... TArgs>
	T& addComponent(TArgs&&... mArgs)
	{
		T* c(new T(std::forward<TArgs>(mArgs)...));
		if constexpr (std::is_base_of_v<LinkComponent, T>) {
			std::unique_ptr<LinkComponent> uPtr{ c };
			components.emplace_back(std::move(uPtr));

			setComponentEntity(c);
			componentArray[GetLinkComponentTypeID<T>()] = c;
			componentBitSet[GetLinkComponentTypeID<T>()] = true;

			c->id = GetLinkComponentTypeID<T>();

			c->init();
			return *c;
		}
		else if constexpr (std::is_base_of_v<NodeComponent, T>) {
			std::unique_ptr<NodeComponent> uPtr{ c };
			components.emplace_back(std::move(uPtr));

		}
		else {
			std::unique_ptr<Component> uPtr{ c };
			components.emplace_back(std::move(uPtr));

		}
		
		setComponentEntity(c);
		componentArray[GetComponentTypeID<T>()] = c;
		componentBitSet[GetComponentTypeID<T>()] = true;
		
		c->id = GetComponentTypeID<T>();

		c->init();
		return *c;
	}

	virtual void setComponentEntity(Component* c) {

	}
	virtual void setComponentEntity(NodeComponent* c) {

	}
	virtual void setComponentEntity(LinkComponent* c) {

	}

	template<typename T> T& GetComponent() const
	{
		if constexpr (std::is_base_of_v<LinkComponent, T>) {
			auto ptr(componentArray[GetLinkComponentTypeID<T>()]);
			return *static_cast<T*>(ptr);
		}

		auto ptr(componentArray[GetComponentTypeID<T>()]);
		return *static_cast<T*>(ptr);
	}

	// for when wanting to add new entities from components
	Manager* getManager() {
		return &manager;
	}

	virtual void addMessage(std::string mMessage) {}


	virtual Entity* getParentEntity() {
		return nullptr;
	}

	virtual void setParentEntity(Entity* pEntity) {}

	virtual void imgui_print() {}

	virtual void imgui_display() {}
};

