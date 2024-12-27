#include "Map.h"
#include "GOS/Components.h"
#include "../GOS/ScriptComponents.h"

extern Manager manager;

int solidTiles[] = {1};

std::vector<TileFeatureCallback> tileFeatures;

Map::Map(std::string tID, int ms, int ts) : texID(tID), mapScale(ms), tileSize(ts) //probably initiallization
{
	scaledSize = ms * ts;

	//TileFeatureCallback addBouncyFeature = &Map::addBouncyTileFeature;

	//tileFeatures.push_back(addBouncyFeature);
}

Map::~Map()
{

}

std::vector<std::vector<int>> Map::generateMap(int width, int height) {
	std::vector<std::vector<int>> map(height, std::vector<int>(width, -1));
	
	return map;
}

void Map::saveMapToCSV(const std::vector<std::vector<int>>& map, const std::string& fileName) {
	std::ofstream file(fileName);

	if (!file.is_open()) {
		std::cerr << "Failed to open file for writing: " << fileName << std::endl;
		return;
	}

	for (const auto& row : map) {
		for (size_t i = 0; i < row.size(); ++i) {
			file << row[i];
			if (i < row.size() - 1)
				file << ",";
		}
		file << "\n";
	}

	file.close();
}

void Map::ProcessLayer(std::fstream& mapFile, void (Map::* addTileFunction)(Entity&, int, int)) {
	
	int x = 0, y = 0;

	int wordNum = 0;
	int arrayTilesIndex = 0;

	int srcX, srcY;

	std::string line, word;

	while (getline(mapFile, line)) //reading tiles (action layer)
	{
		std::stringstream str(line);

		while (getline(str, word, ',')) //this is searching in tilemap
		{
			wordNum = stoi(word);

			if (wordNum >= 0) {
				srcY = (wordNum / 16) * tileSize;
				srcX = (wordNum % 16) * tileSize; //adding tile based on srcX,srcY coordinates

				auto& tile(manager.addEntity());
				(this->*addTileFunction)(tile, x * scaledSize, y * scaledSize);

				for (int i = 0; i < tileFeatures.size(); i++) {
					(this->*tileFeatures[i])(tile, wordNum);
				}
				manager.grid->addEntity(&tile);
			}

			x++;
		}
		x = 0;
		y++;
		if (y == 40) {
			break;
		}
	}
}

glm::ivec2 Map::GetLayerDimensions(std::string filePath)
{
	std::fstream mapFile;

	mapFile.open(filePath);

	if (!mapFile.is_open()) {
		throw std::runtime_error("File stream is not open or in a bad state.");
	}

	std::string line;
	int width = 0, height = 0;

	if (getline(mapFile, line)) {
		std::stringstream ss(line);
		std::string cell;

		while (getline(ss, cell, ',')) {
			width++; 
		}

		height++;  
	}

	while (getline(mapFile, line)) {
		height++;  
	}

	mapFile.clear();  
	mapFile.seekg(0);

	return glm::ivec2(width * scaledSize, height * scaledSize );
}

bool Map::tileHasFeature(Entity& tile, int wordNum, int featureTileArray[], int featureTileArraySize) {
	int arrayTilesIndex = 0;

	for (arrayTilesIndex = 0; arrayTilesIndex < featureTileArraySize; arrayTilesIndex++)
	{
		if (wordNum == featureTileArray[arrayTilesIndex])
		{
			return true;//(this->*addTileFeature)(tile);
		}
	}
	return false;
}

void Map::LoadMap( std::string actionlayerpath)
{
	std::fstream mapFile;

	mapFile.open(actionlayerpath);
	ProcessLayer(mapFile, &Map::AddActionTile);
	mapFile.close();

}

void Map::AddActionTile(Entity &tile, int xpos, int ypos)
{
	//create Node function
	tile.addComponent<TileComponent>(xpos, ypos, tileSize, mapScale); //insert tile and grid and colliders(somehow we refer to background)

	tile.addGroup(Manager::groupActionLayer);
}