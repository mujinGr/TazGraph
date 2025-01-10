#include "Map.h"
#include "GECS/Components.h"
#include "../GECS/ScriptComponents.h"
#include <iostream>

extern Manager manager;
extern std::vector<Entity*>& nodes;
extern std::vector<Entity*>& links;


Map::Map(int ms, int ns) : mapScale(ms), nodeSize(ns) //probably initiallization
{
	scaledSize = ms * ns;
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
	
	file << "Total number of links: " << links.size() << "\n";

	for (auto& entity : links) {
		file << entity->getId() << "\t";
		file << entity->getFromNode()->getId() << "\t";
		file << entity->getToNode()->getId() << "\n";
	}

	file.close();
}

void Map::ProcessFile(std::ifstream& mapFile, void (Map::* addNodeFunction)(Entity&, glm::vec2 mPosition), void (Map::* addLinkFunction)(Entity&, unsigned int fromId, unsigned int toId)) {
	std::string line;
	std::getline(mapFile, line);

	while (std::getline(mapFile, line) && !line.empty()) {
		std::istringstream nodeLine(line);
		int id;
		float x, y;
		int width, height;
		char separator;

		nodeLine >> id; 
		nodeLine >> x >> y;
		nodeLine >> width >> separator >> height;

		auto& node(manager.addEntity<Node>());
		(this->*addNodeFunction)(node, glm::vec2(x, y));

		node.setId(id);

		manager.grid->addEntity(&node);
	}

	std::getline(mapFile, line);

	while (std::getline(mapFile, line)) {
		std::istringstream linkLine(line);
		int id, fromNodeId, toNodeId;

		linkLine >> id >> fromNodeId >> toNodeId;

		auto& link(manager.addEntity<Link>(fromNodeId, toNodeId));
		link.setId(id);
		(this->*addLinkFunction)(link, fromNodeId, toNodeId);
	}
}

void Map::ProcessPythonFile(std::ifstream& mapFile, void (Map::* addNodeFunction)(Entity&, glm::vec2 mPosition), void (Map::* addLinkFunction)(Entity&, unsigned int fromId, unsigned int toId)) {
	
	JsonParser fileParser(mapFile);
	JsonValue rootFromFile = fileParser.parse();
	std::cout << "Parsed JSON from file successfully!" << std::endl;
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
	manager.removeAllEntitiesFromGroup(Manager::groupLinks_0);

	ProcessFile(file, &Map::AddDefaultNode, &Map::AddDefaultLink);

	file.close();
}

void Map::loadPythonMap(const char* fileName) {
	std::string text = "assets/Maps/" + std::string(fileName);
	std::ifstream file(text);

	if (!file.is_open()) {
		std::cerr << "Failed to open file for writing: " << text << std::endl;
		return;
	}

	//todo: loop through the entities and destroy them
	manager.removeAllEntitiesFromGroup(Manager::groupNodes_0);
	manager.removeAllEntitiesFromGroup(Manager::groupLinks_0);

	ProcessPythonFile(file, &Map::AddDefaultNode, &Map::AddDefaultLink);

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

void Map::AddDefaultLink(Entity& link, unsigned int fromNodeId, unsigned int toNodeId)
{
	link.addGroup(Manager::groupLinks_0);
}