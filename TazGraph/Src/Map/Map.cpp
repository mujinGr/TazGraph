#include "Map.h"
#include "GOS/Components.h"
#include "../GOS/ScriptComponents.h"

extern Manager manager;
extern std::vector<Entity*>& nodes;

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


void Map::saveMapAsText(const std::string& fileName) {

	std::string text = "assets/Maps/" + fileName + ".txt";
	std::ofstream file(text);

	if (!file.is_open()) {
		std::cerr << "Failed to open file for writing: " << text << std::endl;
		return;
	}

	file << "Total number of nodes: " << nodes.size() << "\n";

	for (auto& entity : nodes) { 
		
		if (entity->hasComponent<TransformComponent>()) {
			TransformComponent& tc = entity->GetComponent<TransformComponent>();
			file << entity->getId() << "\t"; // id is the index in the vector of entities
			file <<  tc.getPosition().x << " " << tc.getPosition().y << "\t";
			file << tc.width << "x" << tc.height << "\n";
		}
	}

	file << "\n";
	//file << "Total number of links: " << links.size() << "\n";

	//for (auto& entity : links) {

	//	if (entity->hasComponent<TransformComponent>()) {
	//		TransformComponent& tc = entity->GetComponent<TransformComponent>();
	//		file << entity->getId() << "\t"; // id is the index in the vector of entities
	//		file << tc.getPosition().x << " " << tc.getPosition().y << "\t";
	//		file << tc.width << "x" << tc.height << "\n";
	//	}
	//}

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

	tile.addGroup(Manager::groupNodes_0);
}