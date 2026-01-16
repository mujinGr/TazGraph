#pragma once
#include "../IGraphParser.h"

#include <simdump/include/sim_dump/filereader.h>

class SimDumpPathParser : public IGraphParser {
public:
	SimDumpPathParser();
	void readFile(std::string m_fileName) override;

	void writeFile(std::string m_fileName, Manager& manager) override;

	void parse(
		Manager& manager,
		std::function<void(Entity&, glm::vec3)> addNodeFunc,
		std::function<void(Entity&)> addLinkFunc
	) override;

	void parse(Manager& manager,
		sim_dump::FileReader& reader,
		std::vector<NodeEntity*>& nodeEntities,
		std::vector<LinkEntity*>& linkEntities,
		std::function<void(Entity&, glm::vec3)> addNodeFunc,
		std::function<void(Entity&)> addLinkFunc);

	void closeFile() override;

};