#pragma once
#include "../IGraphParser.h"

class TextMapParser : public IGraphParser {
public:
	TextMapParser();
	void readFile(std::string m_fileName) override;

	void writeFile(std::string m_fileName, Manager& manager) override;

	void parse(
		Manager& manager,
		std::function<void(Entity&, glm::vec3)> addNodeFunc,
		std::function<void(Entity&)> addLinkFunc
	) override;

	void closeFile() override;

	void update(float deltaTime) override;

private:
	std::ifstream file;
};