#include "Collision.h"


bool Collision::checkCollisionIsSideways(const SDL_Rect& moving_recA, const SDL_Rect& recB) {

	if (!checkCollision(moving_recA, recB)) {
		return false;
	}

	Collision::storedColliderRect = recB;

#if defined(_WIN32) || defined(_WIN64)
	Collision::overlap.x = min(moving_recA.x + moving_recA.w, recB.x + recB.w) - max(moving_recA.x, recB.x);
	Collision::overlap.y = min(moving_recA.y + moving_recA.h, recB.y + recB.h) - max(moving_recA.y, recB.y);
#else
	Collision::overlap.x = std::min(moving_recA.x + moving_recA.w, recB.x + recB.w) - std::max(moving_recA.x, recB.x);
	Collision::overlap.y = std::min(moving_recA.y + moving_recA.h, recB.y + recB.h) - std::max(moving_recA.y, recB.y);

#endif
	
	//check if it is sideways collision (bool isSidewaysCollision)
	if (Collision::overlap.x < Collision::overlap.y) { //todo: if velocity.y < 0 then have it >= to slide on left wall
		Collision::isSidewaysCollision = true;
		Collision::movingRectColSide = (moving_recA.x < recB.x) ? Collision::ColSide::RIGHT : Collision::ColSide::LEFT;
	}
	else { // Vertical collision
		Collision::movingRectColSide = (moving_recA.y > recB.y) ?  Collision::ColSide::TOP : Collision::ColSide::DOWN;
	}

	Collision::isCollision = true;
	return true;
}

void Collision::moveFromCollision(EmptyEntity& entity) {
	auto& entityTransform = entity.GetComponent<TransformComponent>();
	auto entityCollider = entity.GetComponent<ColliderComponent>();

	switch (Collision::movingRectColSide) {
		case Collision::ColSide::RIGHT: // Move entity to the left of the collider
			entityTransform.setPosition_X(entityTransform.getPosition().x - Collision::overlap.x);
			break;

		case Collision::ColSide::LEFT: // Move entity to the right of the collider
			entityTransform.setPosition_X(entityTransform.getPosition().x + Collision::overlap.x);
			break;

		case Collision::ColSide::DOWN: // Move entity above the collider
			entityTransform.setPosition_Y(entityTransform.getPosition().y - Collision::overlap.y);
			break;

		case Collision::ColSide::TOP: // Move entity below the collider
			entityTransform.setPosition_Y(entityTransform.getPosition().y + Collision::overlap.y);
			entityTransform.setVelocity_Y(0);
			break;

		default:
			break;
	}
}

bool Collision::moveFromOuterBounds() {
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	//if (pos.x < 0) {
	//	entity.GetComponent<TransformComponent>().setPosition_X(0);
	//	return true;
	//}
	//else if (pos.x + entity.GetComponent<TransformComponent>().width > 0/*main_camera2D->worldDimensions.x*/) {
	//	entity.GetComponent<TransformComponent>().setPosition_X(/*main_camera2D->worldDimensions.x */- entity.GetComponent<TransformComponent>().width);
	//	return true;
	//}
	return false;
}
