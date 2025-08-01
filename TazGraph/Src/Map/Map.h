#pragma once
#include <string>
#include <fstream>
#include <sstream>

#include <JsonParser.h>
#include "GECS/Core/GECSEntityTypes.h"

#include "./Parsers/TextMapParser.h"
#include "./Parsers/PythonMapParser.h"
#include "./Parsers/GraphMLMapParser.h"
#include "./Parsers/DOTMapParser.h"

#include <algorithm>
#include <random>
#include <ctime>

class Map
{
public:

	Map(Manager& m_manager, int ms, int ns);
	~Map();

	void saveMapAsText(const char* fileName);

	void loadMap(
		const char* fileName,
		std::function<void(Entity&, glm::vec3)> addNodeFunc,
		std::function<void(Entity&)> addLinkFunc,
		Threader* m_threadPool
	);

	void AddDefaultNode(Entity& node, glm::vec3 mPosition);
	void AddTreeNode(Entity& node, glm::vec3 mPosition);
	void AddDefaultLink(Entity& node);
	void AddTreeLink(Entity& link);

	Manager* manager;
private:
	int mapScale;
	int nodeSize;
	int scaledSize;
};
