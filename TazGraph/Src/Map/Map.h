#pragma once
#include <string>
#include <fstream>
#include <sstream>

#include "GOS/GOSEntityTypes.h"

#include <algorithm>
#include <random>
#include <ctime>

#define ARRAY_SIZE(array) (sizeof(array)/sizeof((array)[0])) 

class Map
{
public:

	Map(std::string tID, int ms, int ts);
	~Map();

	void saveMapAsText(const char* fileName);
	void ProcessFile(std::ifstream& mapFile, void(Map::* addNodeFunction)(Entity&, glm::vec2 mPosition), void(Map::* addLinkFunction)(Entity&, unsigned int fromId, unsigned int toId));
	void loadTextMap(const char* fileName);

	void AddDefaultNode(Entity& node, glm::vec2 mPosition);
	void AddDefaultLink(Entity& node, unsigned int fromNodeId, unsigned int toNodeId);

private:
	int stage = 0;
	std::string texID;
	int mapScale;
	int tileSize;
	int scaledSize;

	bool isMapCompleted = false;
};

using TileFeatureCallback = void (Map::*)(Entity& tile, int wordNum);
