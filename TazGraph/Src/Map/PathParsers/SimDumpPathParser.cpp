#include "./TextPathParser.h"
#include "SimDumpPathParser.h"

SimDumpPathParser::SimDumpPathParser() {}

void SimDumpPathParser::readFile(std::string m_fileName) {
}

void SimDumpPathParser::writeFile(std::string m_fileName, Manager& manager)
{
}


void SimDumpPathParser::parse(Manager& manager,
	std::function<void(Entity&, glm::vec3)> addNodeFunc,
	std::function<void(Entity&)> addLinkFunc)
{

}

void SimDumpPathParser::parse(Manager& manager,
	sim_dump::FileReader& reader,
	std::vector<NodeEntity*>& nodeEntities,
	std::vector<LinkEntity*>& linkEntities,
	std::function<void(Entity&, glm::vec3)> addNodeFunc,
	std::function<void(Entity&)> addLinkFunc)
{
	

}

void SimDumpPathParser::closeFile()
{
}
