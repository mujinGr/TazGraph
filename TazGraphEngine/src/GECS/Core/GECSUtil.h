#pragma once

#include "./GECSManager.h"
#include <unordered_map>

// Map of component names to functions for adding components
static const std::unordered_map<std::string, std::function<void(Entity*)>> addComponentMap = {
	//{"TransformComponent", [](Entity* entity) { entity->addComponent<TransformComponent>(); }},
	{"SpriteComponent", [](Entity* entity) {entity->addToGroup(Manager::groupRenderSprites);entity->addComponent<SpriteComponent>(); }},
	{"ColliderComponent", [](Entity* entity) { entity->addComponent<ColliderComponent>(); }},
	{"Triangle_w_Color", [](Entity* entity) { entity->addComponent<Triangle_w_Color>(); }},
	{"Rectangle_w_Color", [](Entity* entity) { entity->addComponent<Rectangle_w_Color>(); }},
	{"Line_w_Color", [](Entity* entity) { entity->addComponent<Line_w_Color>(); }},
	{"SpringComponent", [](Entity* entity) { entity->addComponent<SpringComponent>(); }},
	{"BoxComponent", [](Entity* entity) { entity->addComponent<BoxComponent>(); }},
	{"SphereComponent", [](Entity* entity) { entity->addComponent<SphereComponent>(); }},
	{"AnimatorComponent", [](Entity* entity) { entity->addComponent<AnimatorComponent>(); }},
	{"MovingAnimatorComponent", [](Entity* entity) { entity->addComponent<MovingAnimatorComponent>(); }},
	{"FlashAnimatorComponent", [](Entity* entity) { entity->addComponent<FlashAnimatorComponent>(); }},
	{"LineFlashAnimatorComponent", [](Entity* entity) { entity->addComponent<LineFlashAnimatorComponent>(); }},
	{"RectangleFlashAnimatorComponent", [](Entity* entity) { entity->addComponent<RectangleFlashAnimatorComponent>(); }},
	{"RigidBodyComponent", [](Entity* entity) { entity->addComponent<RigidBodyComponent>(); }},
	{"PortComponent", [](Entity* entity) { entity->addComponent<PortComponent>(); }},
	{"PortSlotComponent", [](Entity* entity) { entity->addComponent<PortSlotComponent>(); }},
	{"PathLinkerComponent", [](Entity* entity) { entity->addComponent<PathLinkerComponent>(); }},
	{"EmptyPythonCodeComponent", [](Entity* entity) { entity->addComponent<EmptyPythonCodeComponent>(); }},
	{"LinkPythonCodeComponent", [](Entity* entity) { entity->addComponent<LinkPythonCodeComponent>(); }},
};

// Map of component names to functions for removing components
static const std::unordered_map<std::string, std::function<void(Entity*)>> removeComponentMap = {
	//{"TransformComponent", [](Entity* entity) { entity->removeComponent<TransformComponent>(); }},
	{"SpriteComponent", [](Entity* entity) { entity->removeGroup(Manager::groupRenderSprites); entity->removeComponent<SpriteComponent>(); }},
	{"ColliderComponent", [](Entity* entity) { entity->removeComponent<ColliderComponent>(); }},
	{"Triangle_w_Color", [](Entity* entity) { entity->removeComponent<Triangle_w_Color>(); }},
	{"Rectangle_w_Color", [](Entity* entity) { entity->removeComponent<Rectangle_w_Color>(); }},
	{"Line_w_Color", [](Entity* entity) { entity->removeComponent<Line_w_Color>(); }},
	{"SpringComponent", [](Entity* entity) { entity->removeComponent<SpringComponent>(); }},
	{"BoxComponent", [](Entity* entity) { entity->removeComponent<BoxComponent>(); }},
	{"SphereComponent", [](Entity* entity) { entity->removeComponent<SphereComponent>(); }},
	{"AnimatorComponent", [](Entity* entity) { entity->removeComponent<AnimatorComponent>(); }},
	{"MovingAnimatorComponent", [](Entity* entity) { entity->removeComponent<MovingAnimatorComponent>(); }},
	{"FlashAnimatorComponent", [](Entity* entity) { entity->removeComponent<FlashAnimatorComponent>(); }},
	{"LineFlashAnimatorComponent", [](Entity* entity) { entity->removeComponent<LineFlashAnimatorComponent>(); }},
	{"RectangleFlashAnimatorComponent", [](Entity* entity) { entity->removeComponent<RectangleFlashAnimatorComponent>(); }},
	{"RigidBodyComponent", [](Entity* entity) { entity->removeComponent<RigidBodyComponent>(); }},
	{"PortComponent", [](Entity* entity) { entity->removeComponent<PortComponent>(); }},
	{"PortSlotComponent", [](Entity* entity) { entity->removeComponent<PortSlotComponent>(); }},
	{"PathLinkerComponent", [](Entity* entity) { entity->removeComponent<PathLinkerComponent>(); }},
	{"EmptyPythonCodeComponent", [](Entity* entity) { entity->removeComponent<EmptyPythonCodeComponent>(); }},
	{"LinkPythonCodeComponent", [](Entity* entity) { entity->removeComponent<LinkPythonCodeComponent>(); }},
};

static const std::unordered_map<std::string, ComponentID> componentNameToID = {
	{"TransformComponent", GetComponentTypeID<TransformComponent>()},
	{"SpriteComponent", GetComponentTypeID<SpriteComponent>()},
	{"ColliderComponent", GetComponentTypeID<ColliderComponent>()},
	{"Triangle_w_Color", GetComponentTypeID<Triangle_w_Color>()},
	{"Rectangle_w_Color", GetComponentTypeID<Rectangle_w_Color>()},
	{"Line_w_Color", GetLinkComponentTypeID<Line_w_Color>()},
	{"SpringComponent", GetLinkComponentTypeID<SpringComponent>()},
	{"BoxComponent", GetComponentTypeID<BoxComponent>()},
	{"SphereComponent", GetComponentTypeID<SphereComponent>()},
	{"AnimatorComponent", GetComponentTypeID<AnimatorComponent>()},
	{"MovingAnimatorComponent", GetComponentTypeID<MovingAnimatorComponent>()},
	{"FlashAnimatorComponent", GetComponentTypeID<FlashAnimatorComponent>()},
	{"LineFlashAnimatorComponent", GetLinkComponentTypeID<LineFlashAnimatorComponent>()},
	{"RectangleFlashAnimatorComponent", GetComponentTypeID<RectangleFlashAnimatorComponent>()},
	{"RigidBodyComponent", GetComponentTypeID<RigidBodyComponent>()},
	{"PortComponent", GetComponentTypeID<PortComponent>()},
	{"PortSlotComponent", GetComponentTypeID<PortSlotComponent>()},
	{"PathLinkerComponent", GetComponentTypeID<PathLinkerComponent>()},
	{"EmptyPythonCodeComponent", GetComponentTypeID<EmptyPythonCodeComponent>()},
	{"LinkPythonCodeComponent", GetComponentTypeID<LinkPythonCodeComponent>()},
};

// Map of component names to functions for getting components
static const std::unordered_map<std::string, std::function<BaseComponent* (Entity*)>> getComponentMap = {
	{"TransformComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<TransformComponent>(); }},
	{"SpriteComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<SpriteComponent>(); }},
	{"ColliderComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<ColliderComponent>(); }},
	{"Triangle_w_Color", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<Triangle_w_Color>(); }},
	{"Rectangle_w_Color", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<Rectangle_w_Color>(); }},
	{"Line_w_Color", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<Line_w_Color>(); }},
	{"SpringComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<SpringComponent>(); }},
	{"BoxComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<BoxComponent>(); } },
	{"SphereComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<SphereComponent>(); } },
	{"AnimatorComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<AnimatorComponent>(); }},
	{"MovingAnimatorComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<MovingAnimatorComponent>(); }},
	{"FlashAnimatorComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<FlashAnimatorComponent>(); }},
	{"LineFlashAnimatorComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<LineFlashAnimatorComponent>(); }},
	{"RectangleFlashAnimatorComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<RectangleFlashAnimatorComponent>(); }},
	{"RigidBodyComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<RigidBodyComponent>(); }},
	{"PortComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<PortComponent>(); }},
	{"PortSlotComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<PortSlotComponent>(); }},
	{"PathLinkerComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<PathLinkerComponent>(); }},
	{"EmptyPythonCodeComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<EmptyPythonCodeComponent>(); }},
	{"LinkPythonCodeComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<LinkPythonCodeComponent>(); }},
};


// Function to add a component by name
static void AddComponentByName(const std::string& componentName, Entity* entity) {
	auto it = addComponentMap.find(componentName);
	if (it != addComponentMap.end()) {
		it->second(entity);
	}

	entity->getManager()->aboutTo_updateActiveEntities();
}

// Function to remove a component by name
static void RemoveComponentByName(const std::string& componentName, Entity* entity) {
	auto it = removeComponentMap.find(componentName);
	if (it != removeComponentMap.end()) {
		it->second(entity);
	}

	entity->getManager()->aboutTo_updateActiveEntities();
}

// Function to get a component by name
static BaseComponent* getComponentByName(const std::string& componentName, Entity* entity) {
	auto it = getComponentMap.find(componentName);
	if (it != getComponentMap.end()) {
		return it->second(entity);
	}
	return nullptr;
}