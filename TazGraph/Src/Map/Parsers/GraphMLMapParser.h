#pragma once
#include "IMapParser.h"

class GraphMLMapParser : public IMapParser {
public:
	GraphMLMapParser();
	void readFile(std::string m_fileName) override;

	void parse(
		Manager& manager,
		std::function<void(Entity&, glm::vec3)> addNodeFunc,
		std::function<void(Entity&)> addLinkFunc
	) override;

	void closeFile() override;

private:
	tinyxml2::XMLDocument doc;

	tinyxml2::XMLError file;
};
