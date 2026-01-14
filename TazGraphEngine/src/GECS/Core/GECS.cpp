#include "GECS.h"
//#include "TransformComponent.h"
void Entity::addToGroup(Group mGroup)
{
	groupBitSet[mGroup] = true;
}

void Entity::removeGroup(Group mGroup)
{
	groupBitSet[mGroup] = false;
}