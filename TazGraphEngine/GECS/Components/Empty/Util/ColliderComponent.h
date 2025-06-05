#pragma once
#include <string>
#include <unordered_set>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include "../../../Components.h"
#include "../../../UtilComponents.h"
#include "../../../../TextureManager/TextureManager.h"


class ColliderComponent : public NodeComponent //collider -> transform
{
private:
	Manager* _manager = nullptr;
	float _collisionPadding = 100.0f;
	std::unordered_set<Group> _groupChecks;
public:
	glm::vec3 box_collider = glm::vec3(0.0f);

	TransformComponent* transform = nullptr;

	ColliderComponent()
	{
	}

	// todo instead have offset
	ColliderComponent(Manager* manager, glm::vec3 boxCollider_size)
	{
		_manager = manager;
		box_collider = boxCollider_size;
	}

	void init() override
	{
		if (!entity->hasComponent<TransformComponent>()) //todo: problem: having transform on top left grid, not every collider its own
		{
			entity->addComponent<TransformComponent>();
		}
		transform = &entity->GetComponent<TransformComponent>();

	}

	void update(float deltaTime) override
	{
		
		
	}

	void collisionPhysics() {
		glm::vec3 nodePosition = transform->bodyCenter;
		glm::vec3 nodeHalfSize = 0.5f * transform->size;

		for (Group group : _groupChecks) {
			const auto& adjacentEntities = _manager->adjacentEntities(entity, group);

			for (Entity* other : adjacentEntities) {

				auto areEntitiesLinked = [&](NodeEntity* main, Entity* other)
					{
						for (auto& i : main->getOutLinks()) {
							if (i->getToNode() == other) {
								return true;
							}
						}
						return false;
					};

				if (areEntitiesLinked(entity, other)) {
					//continue;
				}

				glm::vec3 otherPosition = other->GetComponent<TransformComponent>().bodyCenter;
				glm::vec3 otherHalfSize = 0.5f * other->GetComponent<TransformComponent>().size;

				glm::vec3 delta = nodePosition - otherPosition;

				float dist = std::max(length(delta), 1e-4f);
				glm::vec3 repulsion = 5000.0f * normalize(delta) / (dist * dist);

				transform->velocity += repulsion;
				other->GetComponent<TransformComponent>().velocity -= repulsion;

			}
		}
	}


	void draw(size_t e_index, PlaneModelRenderer&  batch, TazGraphEngine::Window& window) override
	{
	}

	std::string GetComponentName() override {
		return "ColliderComponent";
	}

	void addCollisionGroup(Group g) {
		_groupChecks.insert(g);
	}

	void removeCollisionGroup(Group g) {
		_groupChecks.erase(g);
	}
};