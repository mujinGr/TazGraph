#pragma once

#include "./GECSManager.h"
#include <unordered_map>

// Map of component names to functions for adding components
static const std::unordered_map<std::string, std::function<void(Entity*)>> addComponentMap = {
    //{"TransformComponent", [](Entity* entity) { entity->addComponent<TransformComponent>(); }},
    {"SpriteComponent", [](Entity* entity) { entity->addComponent<SpriteComponent>(); }},
    {"ColliderComponent", [](Entity* entity) { entity->addComponent<ColliderComponent>(); }},
    {"Triangle_w_Color", [](Entity* entity) { entity->addComponent<Triangle_w_Color>(); }},
    {"Rectangle_w_Color", [](Entity* entity) { entity->addComponent<Rectangle_w_Color>(); }},
    {"Line_w_Color", [](Entity* entity) { entity->addComponent<Line_w_Color>(); }},
    {"AnimatorComponent", [](Entity* entity) { entity->addComponent<AnimatorComponent>(); }},
    {"MovingAnimatorComponent", [](Entity* entity) { entity->addComponent<MovingAnimatorComponent>(); }},
    {"FlashAnimatorComponent", [](Entity* entity) { entity->addComponent<FlashAnimatorComponent>(); }},
    {"LineFlashAnimatorComponent", [](Entity* entity) { entity->addComponent<LineFlashAnimatorComponent>(); }},
    {"UILabel", [](Entity* entity) { entity->addComponent<UILabel>(); }},
    {"ButtonComponent", [](Entity* entity) { entity->addComponent<ButtonComponent>(); }},
    {"RigidBodyComponent", [](Entity* entity) { entity->addComponent<RigidBodyComponent>(); }},
    {"KeyboardControllerComponent", [](Entity* entity) { entity->addComponent<KeyboardControllerComponent>(); }},
    {"GridComponent", [](Entity* entity) { entity->addComponent<GridComponent>(); }},
    {"PollingComponent", [](Entity* entity) { entity->addComponent<PollingComponent>(); }},
};

// Map of component names to functions for removing components
static const std::unordered_map<std::string, std::function<void(Entity*)>> removeComponentMap = {
    //{"TransformComponent", [](Entity* entity) { entity->removeComponent<TransformComponent>(); }},
    {"SpriteComponent", [](Entity* entity) { entity->removeComponent<SpriteComponent>(); }},
    {"ColliderComponent", [](Entity* entity) { entity->removeComponent<ColliderComponent>(); }},
    {"Triangle_w_Color", [](Entity* entity) { entity->removeComponent<Triangle_w_Color>(); }},
    {"Rectangle_w_Color", [](Entity* entity) { entity->removeComponent<Rectangle_w_Color>(); }},
    {"Line_w_Color", [](Entity* entity) { entity->removeComponent<Line_w_Color>(); }},
    {"AnimatorComponent", [](Entity* entity) { entity->removeComponent<AnimatorComponent>(); }},
    {"MovingAnimatorComponent", [](Entity* entity) { entity->removeComponent<MovingAnimatorComponent>(); }},
    {"FlashAnimatorComponent", [](Entity* entity) { entity->removeComponent<FlashAnimatorComponent>(); }},
    {"LineFlashAnimatorComponent", [](Entity* entity) { entity->removeComponent<LineFlashAnimatorComponent>(); }},
    {"UILabel", [](Entity* entity) { entity->removeComponent<UILabel>(); }},
    {"ButtonComponent", [](Entity* entity) { entity->removeComponent<ButtonComponent>(); }},
    {"RigidBodyComponent", [](Entity* entity) { entity->removeComponent<RigidBodyComponent>(); }},
    {"KeyboardControllerComponent", [](Entity* entity) { entity->removeComponent<KeyboardControllerComponent>(); }},
    {"GridComponent", [](Entity* entity) { entity->removeComponent<GridComponent>(); }},
    {"PollingComponent", [](Entity* entity) { entity->removeComponent<PollingComponent>(); }},
};

// Map of component names to functions for getting components
static const std::unordered_map<std::string, std::function<BaseComponent* (Entity*)>> getComponentMap = {
    {"TransformComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<TransformComponent>(); }},
    {"SpriteComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<SpriteComponent>(); }},
    {"ColliderComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<ColliderComponent>(); }},
    {"Triangle_w_Color", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<Triangle_w_Color>(); }},
    {"Rectangle_w_Color", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<Rectangle_w_Color>(); }},
    {"Line_w_Color", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<Line_w_Color>(); }},
    {"AnimatorComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<AnimatorComponent>(); }},
    {"MovingAnimatorComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<MovingAnimatorComponent>(); }},
    {"FlashAnimatorComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<FlashAnimatorComponent>(); }},
    {"LineFlashAnimatorComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<LineFlashAnimatorComponent>(); }},
    {"UILabel", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<UILabel>(); }},
    {"ButtonComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<ButtonComponent>(); }},
    {"RigidBodyComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<RigidBodyComponent>(); }},
    {"KeyboardControllerComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<KeyboardControllerComponent>(); }},
    {"GridComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<GridComponent>(); }},
    {"PollingComponent", [](Entity* entity) -> BaseComponent* { return &entity->GetComponent<PollingComponent>(); }},
};


// Function to add a component by name
void AddComponentByName(const std::string& componentName, Entity* entity) {
    auto it = addComponentMap.find(componentName);
    if (it != addComponentMap.end()) {
        it->second(entity);
    }
}

// Function to remove a component by name
void RemoveComponentByName(const std::string& componentName, Entity* entity) {
    auto it = removeComponentMap.find(componentName);
    if (it != removeComponentMap.end()) {
        it->second(entity);
    }
}

// Function to get a component by name
BaseComponent* getComponentByName(const std::string& componentName, Entity* entity) {
    auto it = getComponentMap.find(componentName);
    if (it != getComponentMap.end()) {
        return it->second(entity);
    }
    return nullptr;
}