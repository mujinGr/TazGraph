#include "Map.h"
#include "GECS/Components.h"
#include "GECS/UtilComponents.h"
#include "../GECS/ScriptComponents.h"
#include <iostream>
#include <limits>


Map::Map(Manager& m_manager, int ms, int ns) : manager(&m_manager), mapScale(ms), nodeSize(ns) //probably initiallization
{
	scaledSize = ms * ns;
}

Map::~Map()
{

}


void Map::saveMapAsText(const char* fileName) {

	auto& nodes(manager->getGroup<NodeEntity>(Manager::groupNodes_0));
	auto& links(manager->getGroup<LinkEntity>(Manager::groupLinks_0));


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
			file << tc.size.x << "x" << tc.size.y << "\n";
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

void Map::ProcessFile(std::ifstream& mapFile, void (Map::* addNodeFunction)(Entity&, glm::vec3 mPosition), void (Map::* addLinkFunction)(Entity&)) {
	std::string line;
	std::getline(mapFile, line); // for first line

	glm::vec2 minPos(FLT_MAX);
	glm::vec2 maxPos(FLT_MIN);
	while (std::getline(mapFile, line) && !line.empty()) {
		std::istringstream nodeLine(line);
		int id;
		float x, y, z;

		nodeLine >> id; 
		nodeLine >> x >> y >> z;

#if defined(_WIN32) || defined(_WIN64)
		minPos.x = min(minPos.x, x);
		minPos.y = min(minPos.y, y);
		maxPos.x = max(maxPos.x, x);
		maxPos.y = max(maxPos.y, y);
#else
		minPos.x = std::min(minPos.x, x);
		minPos.y = std::min(minPos.y, y);
		maxPos.x = std::max(maxPos.x, x);
		maxPos.y = std::max(maxPos.y, y);
#endif
		auto& node(manager->addEntity<Node>());
		(this->*addNodeFunction)(node, glm::vec3(x, y, z));

		node.GetComponent<TransformComponent>().temp_lineParsed = line;
	}

	std::getline(mapFile, line);

	while (std::getline(mapFile, line)) {
		std::istringstream linkLine(line);
		unsigned int id, fromNodeId, toNodeId;

		linkLine >> id >> fromNodeId >> toNodeId;

		auto& link(manager->addEntity<Link>(fromNodeId, toNodeId));
		(this->*addLinkFunction)(link);
	}

	float width = maxPos.x - minPos.x;
	float height = maxPos.y - minPos.y;

	float maxDistance = width > height ? width : height;

	manager->grid->setSize(2 * maxDistance);

	for (auto& node : manager->getGroup<NodeEntity>(Manager::groupNodes_0)) {
		manager->grid->addNode(node, manager->grid->getGridLevel());
	}

	for (auto& link : manager->getGroup<LinkEntity>(Manager::groupLinks_0)) {
		manager->grid->addLink(link, manager->grid->getGridLevel());
	}

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	main_camera2D->setPosition_X((maxPos.x + minPos.x) / 2.0f);
	main_camera2D->setPosition_Y((maxPos.y + minPos.y) / 2.0f);

	float aspect = static_cast<float>(main_camera2D->getCameraDimensions().x) /
		static_cast<float>(main_camera2D->getCameraDimensions().y);

	float zFromWidth = width / 2.0f / (std::tan(glm::radians(45.0f) / 2.0f) * aspect);

	float zFromHeight = height / 2.0f / (std::tan(glm::radians(45.0f) / 2.0f) * aspect);

#if defined(_WIN32) || defined(_WIN64)
	float requiredZ = max(zFromHeight, zFromWidth);
#else
	float requiredZ = std::max(zFromHeight, zFromWidth);

#endif

	main_camera2D->setPosition_Z(-requiredZ);

	main_camera2D->setAimPos(glm::vec3(main_camera2D->eyePos.x, main_camera2D->eyePos.y, main_camera2D->eyePos.z + 1.0f));

}

void Map::ProcessPythonFile(std::ifstream& mapFile,
	void (Map::* addNodeFunction)(Entity&, glm::vec3 mPosition),
	void (Map::* addLinkFunction)(Entity&)
) {

	glm::vec2 minPos(FLT_MAX);
	glm::vec2 maxPos(FLT_MIN);
	
	JsonParser fileParser(mapFile);
	JsonValue rootFromFile = fileParser.parse();
	auto& nodes = rootFromFile.obj["graph"].obj["nodes"];

	for (auto& nodeEntry : nodes.obj) {
		int nodeId = std::stoi(nodeEntry.first);
		auto& nodeInfo = nodeEntry.second;
		float x = nodeInfo.obj["metadata"].obj["x"].num / 10.0f;
		float y = nodeInfo.obj["metadata"].obj["y"].num / 10.0f;

		auto& node = manager->addEntity<Node>();
		glm::vec3 position(x, y, 0);

#if defined(_WIN32) || defined(_WIN64)
		minPos.x = min(minPos.x, x);
		minPos.y = min(minPos.y, y);
		maxPos.x = max(maxPos.x, x);
		maxPos.y = max(maxPos.y, y);
#else
		minPos.x = std::min(minPos.x, x);
		minPos.y = std::min(minPos.y, y);
		maxPos.x = std::max(maxPos.x, x);
		maxPos.y = std::max(maxPos.y, y);
#endif

		(this->*addNodeFunction)(node, position);

		node.GetComponent<TransformComponent>().temp_lineParsed = nodeId;

	}
	auto& links = rootFromFile.obj["graph"].obj["edges"];
	for (auto& linkEntry : links.arr) {
		auto& linkInfo = linkEntry;
		unsigned int fromID = linkInfo.obj["source"].num;
		unsigned int toID	= linkInfo.obj["target"].num;

		auto& link = manager->addEntity<Link>(fromID, toID);
		(this->*addLinkFunction)(link);

		link.GetComponent<Line_w_Color>().temp_lineParsed = std::to_string(fromID) + " - " + std::to_string(toID);

	}
	

	float width = maxPos.x - minPos.x;
	float height = maxPos.y - minPos.y;

	float maxDistance = width > height ? width : height;

	manager->grid->setSize(2 * maxDistance);

	for (auto& node : manager->getGroup<NodeEntity>(Manager::groupNodes_0)) {
		manager->grid->addNode(node, manager->grid->getGridLevel());
	}

	for (auto& link : manager->getGroup<LinkEntity>(Manager::groupLinks_0)) {
		manager->grid->addLink(link, manager->grid->getGridLevel());
	}


	std::cout << "Parsed JSON from file successfully!" << std::endl;

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	main_camera2D->setPosition_X((maxPos.x + minPos.x) / 2.0f);
	main_camera2D->setPosition_Y((maxPos.y + minPos.y) / 2.0f);

	float aspect = static_cast<float>(main_camera2D->getCameraDimensions().x) /
		static_cast<float>(main_camera2D->getCameraDimensions().y);

	float zFromWidth = (maxPos.x - minPos.x) / 2.0f / (std::tan(glm::radians(45.0f) / 2.0f) * aspect);

	float zFromHeight = (maxPos.y - minPos.y) / 2.0f / (std::tan(glm::radians(45.0f) / 2.0f) * aspect);

#if defined(_WIN32) || defined(_WIN64)
	float requiredZ = max(zFromHeight, zFromWidth);
#else
	float requiredZ = std::max(zFromHeight, zFromWidth);

#endif

	main_camera2D->setPosition_Z(-requiredZ);

	main_camera2D->setAimPos(glm::vec3(main_camera2D->eyePos.x, main_camera2D->eyePos.y, main_camera2D->eyePos.z + 1.0f));
}

void Map::loadTextMap(const char* fileName) {

	std::string text = "assets/Maps/" + std::string(fileName);
	std::ifstream file(text);

	if (!file.is_open()) {
		std::cerr << "Failed to open file for writing: " << text << std::endl;
		return;
	}
	
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

	manager->removeAllEntites();

	ProcessPythonFile(file, &Map::AddDefaultNode, &Map::AddDefaultLink);
	
	file.close();
}

void Map::AddDefaultNode(Entity &node, glm::vec3 mPosition)
{
	static int colorOffset = 0;
	colorOffset = (colorOffset + 2) % 256; // Vary color slightly each time

	//create Node function
	node.addComponent<TransformComponent>(mPosition, glm::vec3(10.0f), 1);
	node.addComponent<Rectangle_w_Color>();
	node.GetComponent<Rectangle_w_Color>().setColor(Color(0, colorOffset, 224, 255));

	node.addComponent<RectangleFlashAnimatorComponent>();


	node.addGroup(Manager::groupNodes_0);
}

void Map::AddTreeNode(Entity& node, glm::vec3 mPosition)
{
	static int colorOffset = 0;
	colorOffset = (colorOffset + 2) % 256; // Vary color slightly each time

	//create Node function
	node.addComponent<TransformComponent>(mPosition, glm::vec3(10.0f), 1);
	node.addComponent<Rectangle_w_Color>();
	node.GetComponent<Rectangle_w_Color>().setColor(Color(0, colorOffset, 224, 255));

	node.addComponent<RectangleFlashAnimatorComponent>();
	node.addComponent<PollingComponent>();

	node.addGroup(Manager::groupNodes_0);
}

void Map::AddDefaultLink(Entity& link)
{
	link.addComponent<Line_w_Color>();

	link.GetComponent<Line_w_Color>().setSrcColor(Color(255, 40, 0, 255));
	link.GetComponent<Line_w_Color>().setDestColor(Color(40, 255, 0, 255));

	link.addComponent<LineFlashAnimatorComponent>();

	link.addGroup(Manager::groupLinks_0);
}

void Map::AddTreeLink(Entity& link)
{
	link.addComponent<Line_w_Color>();

	link.GetComponent<Line_w_Color>().setSrcColor(Color(40, 255, 0, 255));
	link.GetComponent<Line_w_Color>().setDestColor(Color(40, 255, 0, 255));

	link.addComponent<LineFlashAnimatorComponent>();

	link.addGroup(Manager::groupLinks_0);
}