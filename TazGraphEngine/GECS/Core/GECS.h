#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <bitset>
#include <array>
#include <unordered_map>
#include <optional>

#include <SDL2/SDL.h>
#include "../../Renderers/PlaneModelRenderer/PlaneModelRenderer.h"
#include "../../Renderers/LineRenderer/LineRenderer.h"
#include "../../Renderers/PlaneColorRenderer/PlaneColorRenderer.h"
#include "../../Camera2.5D/CameraManager.h"
#include "../../Window/Window.h"

#define CULLING_OFFSET 100

class BaseComponent;
class Component;
class NodeComponent;
class LinkComponent;


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


constexpr std::size_t maxComponents = 16;
constexpr std::size_t maxNodeComponents = 16;


inline ComponentID getNewComponentTypeID()
{
	static ComponentID lastID = 0u;
	return lastID++;
}

inline ComponentID getNewNodeComponentTypeID()
{
	static ComponentID lastID_nodeC = maxComponents;
	return lastID_nodeC++;
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

template <typename T> inline ComponentID GetNodeComponentTypeID() noexcept
{
	static ComponentID typeID = getNewNodeComponentTypeID(); // Any NodeComponent type
	return typeID;
}

template <typename T> inline ComponentID GetLinkComponentTypeID() noexcept
{
	static ComponentID typeID = getNewLinkComponentTypeID(); // typeID is unique for each function type T and only initialized once.
	return typeID;
}


constexpr std::size_t maxGroups = 16;

using ComponentBitSet = std::bitset<maxComponents + maxNodeComponents>;

using ComponentArray = std::array<BaseComponent*, maxComponents>;
using NodeComponentArray = std::array<BaseComponent*, maxNodeComponents>;


using GroupBitSet = std::bitset<maxGroups>;

namespace Layer {
	enum layerIndexes : std::size_t
	{
		action,
		menubackground
	};
}


const std::unordered_map<layer, float> layerNames = {
	{Layer::action , 0.0f},
	{Layer::menubackground, -100.0f}

};

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

//todo seperate draw calls for components
class Component : public BaseComponent {
public:
	EmptyEntity* entity;
};

class NodeComponent : public BaseComponent
{
public:
	NodeEntity* entity;

	virtual void draw(PlaneModelRenderer& batch, TazGraphEngine::Window& window) override {}
	virtual void draw(PlaneColorRenderer& batch, TazGraphEngine::Window& window) override {}

};

class LinkComponent : public BaseComponent
{
public:
	LinkEntity* entity;

	virtual void draw(LineRenderer& batch, TazGraphEngine::Window& window) override {}
};


class Entity
{
private:
	unsigned int id;

	bool active = true; // false if about to delete
	bool hidden = false; // true if not do updates
	
	ComponentArray componentArray;	//create 2 arrays, this is for the fast access
	std::optional<NodeComponentArray> nodeSpecific_componentArray;

	ComponentBitSet componentBitSet;

	GroupBitSet groupBitSet;

protected:
	Manager& manager;
public:
	std::unordered_map<std::string,EmptyEntity*> children;
	std::unordered_map<std::string, NodeEntity*> nodeChildren;
	std::unordered_map<std::string, LinkEntity*> linkChildren;


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
		if constexpr (std::is_base_of_v<NodeComponent, T>) {
			return this && componentBitSet[GetNodeComponentTypeID<T>()];
		}
		if constexpr (std::is_base_of_v<LinkComponent, T>) {
			return this && componentBitSet[GetLinkComponentTypeID<T>()];
		}
		else {
			return this && componentBitSet[GetComponentTypeID<T>()];
		}
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
			nodeSpecific_componentArray[GetNodeComponentTypeID<T>()] = c;
			componentBitSet[GetNodeComponentTypeID<T>()] = true;
			c->id = GetNodeComponentTypeID<T>();
		}
		else if constexpr (std::is_base_of_v<LinkComponent, T>) {
			componentArray[GetLinkComponentTypeID<T>()] = c;
			componentBitSet[GetLinkComponentTypeID<T>()] = true;
			c->id = GetLinkComponentTypeID<T>();
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
			auto ptr(nodeSpecific_componentArray[GetNodeComponentTypeID<T>()]);
			return *static_cast<T*>(ptr);
		}
		else if constexpr (std::is_base_of_v<LinkComponent, T>) {
			auto ptr(componentArray[GetLinkComponentTypeID<T>()]);
			return *static_cast<T*>(ptr);
		}
		else {
			auto ptr(componentArray[GetComponentTypeID<T>()]);
			return *static_cast<T*>(ptr);
		}
	}

	// for when wanting to add new entities from components
	Manager* getManager() {
		return &manager;
	}

	virtual void addMessage(std::string mMessage) {}

	virtual void imgui_print() {}

	virtual void imgui_display() {}


	static float getLayerDepth(layer mLayer) {
		return layerNames.at(mLayer);
	}
};





