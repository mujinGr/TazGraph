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


void Map::loadMap(
	const char* fileName,
	std::function<void(Entity&, glm::vec3)> addNodeFunc,
	std::function<void(Entity&)> addLinkFunc,
	Threader* m_threadPool
) {
	std::string text = "assets/Maps/" + std::string(fileName);

	std::unique_ptr<IMapParser> processor;
	if (text.find(".py") != std::string::npos) {
		processor = std::make_unique<PythonMapParser>();
	}
	else if (text.find(".graphml") != std::string::npos) {
		processor = std::make_unique<GraphMLMapParser>();
	}
	else if (text.find(".dot") != std::string::npos) {
		processor = std::make_unique<DOTMapParser>();
	}
	else {
		processor = std::make_unique<TextMapParser>();
	}

	manager->removeAllEntites();

	processor->setThreader(*m_threadPool);

	processor->readFile(text);
	processor->parse(*manager, addNodeFunc, addLinkFunc);
	processor->closeFile();

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
}

void Map::AddDefaultLink(Entity& link)
{
	link.addComponent<Line_w_Color>();

	link.GetComponent<Line_w_Color>().setSrcColor(Color(255, 40, 0, 255));
	link.GetComponent<Line_w_Color>().setDestColor(Color(40, 255, 0, 255));

	link.addComponent<LineFlashAnimatorComponent>();
}

void Map::AddTreeLink(Entity& link)
{
	link.addComponent<Line_w_Color>();

	link.GetComponent<Line_w_Color>().setSrcColor(Color(40, 255, 0, 255));
	link.GetComponent<Line_w_Color>().setDestColor(Color(40, 255, 0, 255));

	link.addComponent<LineFlashAnimatorComponent>();
}