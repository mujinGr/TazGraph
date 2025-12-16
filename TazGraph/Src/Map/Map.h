#pragma once
#include <string>
#include <fstream>
#include <sstream>

#include <JsonParser.h>

#include "TazGraphEngine.h"

#include "./Parsers/TextMapParser.h"
#include "./Parsers/PythonMapParser.h"
#include "./Parsers/GraphMLMapParser.h"
#include "./Parsers/DOTMapParser.h"
#include "./Parsers/SimDumpMapParser.h"

#include "./PathParsers/TextPathParser.h"

#include <algorithm>
#include <random>
#include <ctime>

/**
 * @defgroup mapLoading Map Loading
 * @brief Architecture and workflow for loading maps and paths.
 *
 * The map loading system separates file parsing from entity creation.
 * Each parser derives from IMapParser and interacts with Map.cpp via callbacks.
 */

class Map
{
public:

	Map(Manager& m_manager, int ms, int ns);
	~Map();

	void saveMap(const char* fileName);

	void loadMap(
		const char* fileName,
		std::function<void(Entity&, glm::vec3)> addNodeFunc,
		std::function<void(Entity&)> addLinkFunc,
		Threader* m_threadPool
	);

	void loadPaths(const char* fileName, std::function<void(Entity&, glm::vec3)> addNodeFunc, std::function<void(Entity&)> addLinkFunc, Threader* m_threadPool);

	Manager* manager;
private:
	int mapScale;
	int nodeSize;
	int scaledSize;
};
