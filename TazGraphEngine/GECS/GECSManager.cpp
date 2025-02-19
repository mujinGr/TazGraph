#include "GECSManager.h"

std::string Manager::getGroupName(Group mGroup) const {
	return groupNames.at(mGroup);
}

float Manager::getLayerDepth(Layer mLayer) const {
	return layerNames.at(mLayer);
}