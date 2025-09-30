#include "Map.h"
#include "GECS/Components.h"
#include "GECS/UtilComponents.h"
#include "../GECS/ScriptComponents.h"

#include "../AssetManager/AssetManager.h"
#include <iostream>
#include <limits>


Map::Map(Manager& m_manager, int ms, int ns) : manager(&m_manager), mapScale(ms), nodeSize(ns) //probably initiallization
{
	scaledSize = ms * ns;
}

Map::~Map()
{

}


void Map::saveMap(const char* fileName) {

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
	else if (text.find(".txt") != std::string::npos) {
		processor = std::make_unique<TextMapParser>();
	}
	else if (text.find(".simdmp") != std::string::npos) {
		processor = std::make_unique<SimDumpMapParser>();
	}
	else {
		manager->removeAllEntites();
		return;
	}

	processor->writeFile(text, *manager);
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
	else if (text.find(".txt") != std::string::npos) {
		processor = std::make_unique<TextMapParser>();
	}
	else if (text.find(".simdmp") != std::string::npos) {
		processor = std::make_unique<SimDumpMapParser>();

		manager->removeAllEntites();

		processor->setThreader(*m_threadPool);

		processor->readFile(text);
		processor->parse(*manager, AssetManager::AddSimulationNode, AssetManager::AddSimulationLink);
		processor->closeFile();
		return;
	}
	else {
		manager->removeAllEntites();
		return;
	}

	manager->removeAllEntites();

	processor->setThreader(*m_threadPool);

	processor->readFile(text);
	processor->parse(*manager, addNodeFunc, addLinkFunc);
	processor->closeFile();

}

void Map::loadPaths(
	const char* fileName,
	std::function<void(Entity&, glm::vec3)> addNodeFunc,
	std::function<void(Entity&)> addLinkFunc,
	Threader* m_threadPool
) {
	std::string text = "assets/Paths/" + std::string(fileName);

	std::unique_ptr<IMapParser> processor;

	processor = std::make_unique<TextPathParser>();

	processor->readFile(text);
	processor->parse(*manager, addNodeFunc, addLinkFunc);
	processor->closeFile();

}

