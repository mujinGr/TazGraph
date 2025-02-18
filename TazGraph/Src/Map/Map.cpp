#include "Map.h"
#include "GECS/Components.h"
#include "GECS/UtilComponents.h"
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
			file << tc.bodyDims.w << "x" << tc.bodyDims.h << "\n";
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

void Map::ProcessFile(std::ifstream& mapFile, void (Map::* addNodeFunction)(Entity&, glm::vec2 mPosition), void (Map::* addLinkFunction)(Entity&)) {
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

		manager.grid->addNode(&node, manager.grid->getGridLevel());
	}

	std::getline(mapFile, line);

	while (std::getline(mapFile, line)) {
		std::istringstream linkLine(line);
		int id, fromNodeId, toNodeId;

		linkLine >> id >> fromNodeId >> toNodeId;

		auto& link(manager.addEntity<Link>(fromNodeId, toNodeId));
		link.setId(id);
		(this->*addLinkFunction)(link);

		manager.grid->addLink(&link, manager.grid->getGridLevel());
	}
}

void Map::ProcessPythonFile(std::ifstream& mapFile,
	void (Map::* addNodeFunction)(Entity&, glm::vec2 mPosition),
	void (Map::* addLinkFunction)(Entity&)
) {
	
	JsonParser fileParser(mapFile);
	JsonValue rootFromFile = fileParser.parse();
	auto& nodes = rootFromFile.obj["graph"].obj["nodes"];
	for (auto& nodeEntry : nodes.obj) {
		int nodeId = std::stoi(nodeEntry.first);
		auto& nodeInfo = nodeEntry.second;
		float x = nodeInfo.obj["metadata"].obj["x"].num / 10.0f;
		float y = nodeInfo.obj["metadata"].obj["y"].num / 10.0f;

		auto& node = manager.addEntity<Node>();
		glm::vec2 position(x, y);
		(this->*addNodeFunction)(node, position);

		manager.grid->addNode(&node, manager.grid->getGridLevel());
	}
	auto& links = rootFromFile.obj["graph"].obj["edges"];
	for (auto& linkEntry : links.arr) {
		auto& linkInfo = linkEntry;
		unsigned int fromID = linkInfo.obj["source"].num;
		unsigned int toID	= linkInfo.obj["target"].num;

		auto& link = manager.addEntity<Link>(fromID, toID);
		(this->*addLinkFunction)(link);

		manager.grid->addLink(&link, manager.grid->getGridLevel());
	}
	std::cout << "Parsed JSON from file successfully!" << std::endl;
}

void Map::loadTextMap(const char* fileName) {

	std::string text = "assets/Maps/" + std::string(fileName);
	std::ifstream file(text);

	if (!file.is_open()) {
		std::cerr << "Failed to open file for writing: " << text << std::endl;
		return;
	}
	
	manager.removeAllEntites();
	
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

	manager.removeAllEntites();

	ProcessPythonFile(file, &Map::AddDefaultNode, &Map::AddDefaultLink);

	file.close();
}

void Map::AddDefaultNode(Entity &node, glm::vec2 mPosition)
{
	//create Node function
	node.addComponent<TransformComponent>(mPosition, Manager::actionLayer, glm::ivec2(10, 10), 1);
	node.addComponent<Rectangle_w_Color>();
	node.GetComponent<Rectangle_w_Color>().color = Color(0, 40, 224, 255);

	node.GetComponent<TransformComponent>().update(0.0f); // update children positions

	node.addGroup(Manager::groupNodes_0);
}

void Map::AddDefaultLink(Entity& link)
{
	link.addComponent<Line_w_Color>();

	link.GetComponent<Line_w_Color>().setSrcColor(Color(255, 40, 0, 255));
	link.GetComponent<Line_w_Color>().setDestColor(Color(255, 40, 0, 255));

	link.addComponent<LineFlashAnimatorComponent>();

	link.addGroup(Manager::groupLinks_0);
}