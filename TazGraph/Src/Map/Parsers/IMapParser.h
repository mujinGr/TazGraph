#pragma once

#include <algorithm>
#include <random>
#include <ctime>

#include "JsonParser/JsonParser.h"
#include "GECS/Core/GECSEntityTypes.h"

class IMapParser {
public:
	virtual ~IMapParser() = default;
	virtual void parse(Manager& manager,
		std::function<void(Entity&, glm::vec3)> addNodeFunc,
		std::function<void(Entity&)> addLinkFunc) = 0;
};