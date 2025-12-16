#pragma once

#include <algorithm>
#include <random>
#include <ctime>

#include <JsonParser.h> //for json
#include <tinyxml2.h> // for graphml
#include <DotParser.h> // for dot/graphviz

#include "TazGraphEngine.h"

struct ParsedNode {
	int id;
	glm::vec3 pos;
};

struct ParsedLink {
	int id;
	int fromId, toId;
};

struct DetailParsedLink {
	int id;
	int fromId, toId;
	NodeEntity* from = nullptr;
	NodeEntity* to = nullptr;
};

/**
 * @ingroup mapLoading
 * @brief Abstract base class for all map parsers.
 *
 * Additionally, parsers can use a @ref Threader "Threader" to enhance performance
 * with multithreaded parsing.
 * 
 * (see @ref fileFormats "Supported File Formats")
 */

class IMapParser {
public:
	virtual ~IMapParser() = default;

	virtual void readFile(std::string m_fileName) = 0;

	virtual void writeFile(std::string m_fileName, Manager& manager) = 0;

	virtual void parse(
		Manager& manager,
		std::function<void(Entity&, glm::vec3)> addNodeFunc,
		std::function<void(Entity&)> addLinkFunc) = 0;;

	virtual void closeFile() = 0;


	Threader* _threader = nullptr;

	void setThreader(Threader& mthreader) {
		_threader = &mthreader;
	}
};