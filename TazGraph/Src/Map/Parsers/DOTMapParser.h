#pragma once
#include "IMapParser.h"

class DOTMapParser : public IMapParser {
public:
	explicit DOTMapParser(std::ifstream& file);
	void parse(
		Manager& manager,
		std::function<void(Entity&, glm::vec3)> addNodeFunc,
		std::function<void(Entity&)> addLinkFunc
	) override;

private:
	std::ifstream& file;
};
