#pragma once
#include "IMapParser.h"

class DOTMapParser : public IMapParser {
public:
	DOTMapParser();
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
