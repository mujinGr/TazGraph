#include "GECS.h"
//#include "TransformComponent.h"
void Entity::addGroup(Group mGroup)
{
	groupBitSet[mGroup] = true;
}

void Entity::removeGroup(Group mGroup)
{
	groupBitSet[mGroup] = false;
}