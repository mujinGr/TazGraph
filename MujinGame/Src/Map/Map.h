#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include "ECS\ECSManager.h"

#include <algorithm>
#include <random>
#include <ctime>

#define ARRAY_SIZE(array) (sizeof(array)/sizeof((array)[0])) 

class Map
{
public:

	Map(std::string tID, int ms, int ts);
	~Map();

	std::vector<std::vector<int>> generateMap(int width, int height);

	void saveMapToCSV(const std::vector<std::vector<int>>& map, const std::string& fileName);
	void ProcessLayer(std::fstream& mapFile, void (Map::* addTileFunction)(Entity&, int, int));
	glm::ivec2 GetLayerDimensions(std::string mapFile);
	void LoadMap(std::string actionlayerpath);
	void AddActionTile(Entity& tile, int xpos, int ypos);

	bool tileHasFeature(Entity& tile, int wordNum, int featureTileArray[], int featureTileArraySize); //, void (Map::* addTileFeature)(Entity&)
	//void DrawMap();

	void setMapCompleted(bool isCompleted);
	bool getMapCompleted();

	void resetMap();

	int getStage();
private:
	int stage = 0;
	std::string texID;
	int mapScale;
	int tileSize;
	int scaledSize;

	bool isMapCompleted = false;
};

using TileFeatureCallback = void (Map::*)(Entity& tile, int wordNum);
