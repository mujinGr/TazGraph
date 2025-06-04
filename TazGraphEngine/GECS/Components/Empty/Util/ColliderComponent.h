#pragma once
#include <string>
#include <unordered_set>
#include <SDL2/SDL.h>
#include "../../../Components.h"
#include "../../../UtilComponents.h"
#include "../../../../TextureManager/TextureManager.h"


class ColliderComponent : public Component //collider -> transform
{
private:
	Manager* _manager = nullptr;

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
				glm::vec3 otherPosition = other->GetComponent<TransformComponent>().bodyCenter;
				glm::vec3 otherHalfSize = 0.5f * other->GetComponent<TransformComponent>().size;

				if (checkCollision3D(nodePosition, nodeHalfSize, otherPosition, otherHalfSize)) {
					glm::vec3 delta = nodePosition - otherPosition;

					glm::vec3 intersect = (nodeHalfSize + otherHalfSize) - glm::abs(delta);

					if (intersect.x > 0 && intersect.y > 0 && intersect.z > 0) {
						glm::vec3 direction(0.0f);
						glm::vec3 separation(0.0f);

						if (intersect.x < intersect.y && intersect.x < intersect.z) {
							direction = glm::vec3((delta.x < 0 ? -1.0f : 1.0f), 0, 0);
							separation = glm::vec3(direction.x * intersect.x * 0.5f, 0, 0);
						}
						else if (intersect.y < intersect.z) {
							direction = glm::vec3(0, (delta.y < 0 ? -1.0f : 1.0f), 0);
							separation = glm::vec3(0, direction.y * intersect.y * 0.5f, 0);
						}
						else {
							direction = glm::vec3(0, 0, (delta.z < 0 ? -1.0f : 1.0f));
							separation = glm::vec3(0, 0, direction.z * intersect.z * 0.5f);
						}

						transform->position += separation;
						other->GetComponent<TransformComponent>().position -= separation;
					}


				}
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