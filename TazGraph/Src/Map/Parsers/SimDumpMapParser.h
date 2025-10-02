#pragma once

#include "../IMapParser.h"
#include "simdump/include/sim_dump/filereader.h"
#include "simdump/include/sim_dump/filewriter.h"

#include "../PathParsers/SimDumpPathParser.h"
#include <DataManager/DataManager.h>

using namespace sim_dump;

class SimDumpMapParser : public IMapParser {
public:
	SimDumpMapParser();
	void readFile(std::string m_fileName) override;

	void writeFile(std::string m_fileName, Manager& manager) override;

	void parse(
		Manager& manager,
		std::function<void(Entity&, glm::vec3)> addNodeFunc,
		std::function<void(Entity&)> addLinkFunc
	) override;

	void closeFile() override;

private:
	void createSteps(sim_dump::FileReader& reader, Manager& manager,
		std::function<void(Entity&, glm::vec3)> addNodeFunc,
		std::function<void(Entity&)> addLinkFunc);

	std::string fileName;
	std::ifstream file;

	std::vector<SimulationStep::TazSimulationNode> parsedNodes;
	std::vector<ParsedLink> parsedLinks;

};