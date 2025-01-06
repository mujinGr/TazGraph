#include "Map.h"
#include "GOS/Components.h"
#include "../GOS/ScriptComponents.h"
#include <iostream>

extern Manager manager;
extern std::vector<Entity*>& nodes;

Map::Map(std::string tID, int ms, int ts) : texID(tID), mapScale(ms), tileSize(ts) //probably initiallization
{
	scaledSize = ms * ts;
}

Map::~Map()
{

}


void Map::saveMapAsText(const char* fileName) {

	std::string text = "assets/Maps/" + std::string(fileName);
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

void Map::ProcessFile(std::ifstream& mapFile, void (Map::* addNodeFunction)(Entity&, glm::vec2 mPosition)) {
	std::string line;
	std::getline(mapFile, line);

	while (std::getline(mapFile, line)) {
		std::istringstream nodeLine(line);
		int id;
		float x, y;
		int width, height;
		char separator;

		nodeLine >> id; // Read entity ID
		nodeLine >> x >> y; // Read position x, y
		nodeLine >> width >> separator >> height; // Read dimensions width x height

		auto& tile(manager.addEntity());
		(this->*addNodeFunction)(tile, glm::vec2(x, y));

		manager.grid->addEntity(&tile);

		std::cout << "Loaded entity ID " << id << " at position (" << x << ", " << y
			<< ") with size " << width << "x" << height << std::endl;
	}
}

void Map::loadTextMap(const char* fileName) {

	std::string text = "assets/Maps/" + std::string(fileName);
	std::ifstream file(text);

	if (!file.is_open()) {
		std::cerr << "Failed to open file for writing: " << text << std::endl;
		return;
	}
	//todo: loop through the entities and destroy them
	manager.removeAllEntitiesFromGroup(Manager::groupNodes_0);
	ProcessFile(file, &Map::AddDefaultNode);

	file.close();
}

void Map::AddDefaultNode(Entity &node, glm::vec2 mPosition)
{
	//create Node function
	node.addComponent<TransformComponent>(mPosition, Manager::actionLayer, glm::ivec2(32, 32), 1);
	node.addComponent<Rectangle_w_Color>();
	node.GetComponent<Rectangle_w_Color>().color = Color(0, 40, 224, 255);

	node.addGroup(Manager::groupNodes_0);
}