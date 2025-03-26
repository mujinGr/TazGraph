#pragma once

#include "./GECSManager.h"

void AddComponentByName(const std::string& componentName, Entity* entity) {
	// cant remove or add Transform, its basic for selection of entity
	/*if (componentName == "TransformComponent") addComponent<TransformComponent>();
	else*/
	if (componentName == "TransformComponent") entity->addComponent<TransformComponent>();
	else if (componentName == "SpriteComponent") entity->addComponent<SpriteComponent>();
	else if (componentName == "Rectangle_w_Color") entity->addComponent<Rectangle_w_Color>();

}

void RemoveComponentByName(const std::string& componentName, Entity* entity) {
	// cant remove or add Transform, its basic for selection of entity
	//if (componentName == "TransformComponent") entity->removeComponent<TransformComponent>();

	if (componentName == "TransformComponent") entity->removeComponent<TransformComponent>();
	else if(componentName == "SpriteComponent") entity->removeComponent<SpriteComponent>();
	else if (componentName == "Rectangle_w_Color") entity->removeComponent<Rectangle_w_Color>();


}

BaseComponent* getComponentByName(const std::string& componentName, Entity* entity) {
	// cant remove or add Transform, its basic for selection of entity
	//if (componentName == "TransformComponent") entity->removeComponent<TransformComponent>();

	if (componentName == "TransformComponent") return &entity->GetComponent<TransformComponent>();
	else if (componentName == "SpriteComponent") return &entity->GetComponent<SpriteComponent>();
	else if (componentName == "Rectangle_w_Color") return &entity->GetComponent<Rectangle_w_Color>();

	return nullptr;
}