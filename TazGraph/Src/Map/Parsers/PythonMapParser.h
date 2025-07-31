#pragma once
#include "IMapParser.h"

class PythonMapParser : public IMapParser {
public:
	PythonMapParser();
	void readFile(std::string m_fileName) override;

	void parse(
		Manager& manager,
		std::function<void(Entity&, glm::vec3)> addNodeFunc,
		std::function<void(Entity&)> addLinkFunc
	) override;

	void closeFile() override;

private:
	std::ifstream file;
};
