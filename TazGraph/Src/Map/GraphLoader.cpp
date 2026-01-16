#include "GraphLoader.h"

#include "TazGraphEngine.h"

#include "../AssetManager/AssetManager.h"
#include <iostream>
#include <limits>


GraphLoader::GraphLoader(Manager& m_manager, int ms, int ns) : manager(&m_manager), mapScale(ms), nodeSize(ns) //probably initiallization
{
	scaledSize = ms * ns;
}

GraphLoader::~GraphLoader()
{

}


void GraphLoader::saveMap(const char* fileName) {

	std::string text = "assets/Maps/" + std::string(fileName);

	std::unique_ptr<IGraphParser> processor;
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


void GraphLoader::loadMap(
	const char* fileName,
	std::function<void(Entity&, glm::vec3)> addNodeFunc,
	std::function<void(Entity&)> addLinkFunc,
	Threader* m_threadPool
) {
	std::string filePath = std::string(fileName);

	// If the path doesn't contain a directory separator, assume it's in assets/Maps/
	if (filePath.find('/') == std::string::npos && filePath.find('\\') == std::string::npos) {
		filePath = "assets/Maps/" + filePath;
	}

	std::unique_ptr<IGraphParser> processor;
	if (filePath.find(".py") != std::string::npos) {
		processor = std::make_unique<PythonMapParser>();
	}
	else if (filePath.find(".graphml") != std::string::npos) {
		processor = std::make_unique<GraphMLMapParser>();
	}
	else if (filePath.find(".dot") != std::string::npos) {
		processor = std::make_unique<DOTMapParser>();
	}
	else if (filePath.find(".txt") != std::string::npos) {
		processor = std::make_unique<TextMapParser>();
	}
	else if (filePath.find(".simdmp") != std::string::npos) {
		processor = std::make_unique<SimDumpMapParser>();

		manager->removeAllEntites();

		processor->setThreader(*m_threadPool);

		processor->readFile(filePath);
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

	processor->readFile(filePath);
	processor->parse(*manager, addNodeFunc, addLinkFunc);
	processor->closeFile();

}

void GraphLoader::loadPaths(
	const char* fileName,
	std::function<void(Entity&, glm::vec3)> addNodeFunc,
	std::function<void(Entity&)> addLinkFunc,
	Threader* m_threadPool
) {
	std::string text = "assets/Paths/" + std::string(fileName);

	std::unique_ptr<IGraphParser> processor;

	processor = std::make_unique<TextPathParser>();

	processor->readFile(text);
	processor->parse(*manager, addNodeFunc, addLinkFunc);
	processor->closeFile();

}

