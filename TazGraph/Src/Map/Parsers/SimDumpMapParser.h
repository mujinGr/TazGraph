#pragma once

#include "../IMapParser.h"
#include "simdump/include/sim_dump/filereader.h"

using namespace sim_dump;

class SimDumpMapParser : public IMapParser {
public:
	SimDumpMapParser();
	void readFile(std::string m_fileName) override;

	void parse(
		Manager& manager,
		std::function<void(Entity&, glm::vec3)> addNodeFunc,
		std::function<void(Entity&)> addLinkFunc
	) override;

	void closeFile() override;

private:
	std::ifstream file;
	std::string fileName;
};