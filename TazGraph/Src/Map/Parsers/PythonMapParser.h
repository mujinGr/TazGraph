#pragma once
#include "IMapParser.h"

class PythonMapParser : public IMapParser {
public:
	explicit PythonMapParser(std::ifstream& file);
	void parse(
		Manager& manager,
		std::function<void(Entity&, glm::vec3)> addNodeFunc,
		std::function<void(Entity&)> addLinkFunc
	) override;

private:
	std::ifstream& file;
};
