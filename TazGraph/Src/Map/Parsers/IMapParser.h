#pragma once

#include <algorithm>
#include <random>
#include <ctime>

#include "JsonParser/JsonParser.h"
#include "GECS/Core/GECSEntityTypes.h"

struct ParsedNode {
	int id;
	glm::vec3 pos;
};

struct ParsedLink {
	int id;
	int fromId, toId;
};

struct DetailParsedLink {
	int id;
	int fromId, toId;
	NodeEntity* from = nullptr;
	NodeEntity* to = nullptr;
};


class IMapParser {
public:
	virtual ~IMapParser() = default;
	virtual void parse(Manager& manager,
		std::function<void(Entity&, glm::vec3)> addNodeFunc,
		std::function<void(Entity&)> addLinkFunc) = 0;

	Threader* _threader = nullptr;

	void setThreader(Threader& mthreader) {
		_threader = &mthreader;
	}
};