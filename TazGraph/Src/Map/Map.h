#pragma once
#include <string>
#include <fstream>
#include <sstream>

#include "JsonParser/JsonParser.h"
#include "GECS/Core/GECSEntityTypes.h"

#include <algorithm>
#include <random>
#include <ctime>

class Map
{
public:

	Map(int ms, int ns);
	~Map();

	void saveMapAsText(const char* fileName);
	void ProcessFile(std::ifstream& mapFile, void(Map::* addNodeFunction)(Entity&, glm::vec2 mPosition), void(Map::* addLinkFunction)(Entity&));
	void ProcessPythonFile(std::ifstream& mapFile, void(Map::* addNodeFunction)(Entity&, glm::vec2 mPosition), void(Map::* addLinkFunction)(Entity&));
	void loadTextMap(const char* fileName);

	void loadPythonMap(const char* fileName);

	void AddDefaultNode(NodeEntity& node, glm::vec2 mPosition);
	void AddDefaultLink(LinkEntity& node);

private:
	int mapScale;
	int nodeSize;
	int scaledSize;
};
