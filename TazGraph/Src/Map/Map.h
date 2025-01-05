#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include "GOS/GOSManager.h"

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
	void ProcessFile(std::ifstream& mapFile, void(Map::* addNodeFunction)(Entity&, int, int));
	void loadTextMap(const char* fileName);
	void AddDefaultNode(Entity& node, int xpos, int ypos);

private:
	int stage = 0;
	std::string texID;
	int mapScale;
	int tileSize;
	int scaledSize;

	bool isMapCompleted = false;
};

using TileFeatureCallback = void (Map::*)(Entity& tile, int wordNum);
