#include "GECSManager.h"
//#include "TransformComponent.h"
void Entity::addGroup(Group mGroup)
{
	groupBitSet[mGroup] = true;
}