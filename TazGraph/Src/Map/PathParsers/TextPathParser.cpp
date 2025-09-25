#include "./TextPathParser.h"

TextPathParser::TextPathParser() {}

void TextPathParser::readFile(std::string m_fileName) {
	file.open(m_fileName);

	if (!file.is_open()) {
		std::cerr << "Failed to open file for reading: " << m_fileName << std::endl;
		return;
	}
}

void TextPathParser::closeFile() {
	file.close();
}

void TextPathParser::parse(Manager& manager,
	std::function<void(Entity&, glm::vec3)> addNodeFunc,
	std::function<void(Entity&)> addLinkFunc)
{
	std::string line;

	std::vector<std::string> linkLines;
	while (std::getline(file, line)) {
		linkLines.push_back(line);
	}



	std::vector<Entity*> linkEntities;

	for (const std::string& line : linkLines) {
		std::stringstream ss(line);

		std::string idsPart, colorPart;
		float width = 1.0f;

		ss >> idsPart >> colorPart >> width;

		// Parse IDs (split by '-')
		std::vector<int> ids;
		std::stringstream idStream(idsPart);
		std::string token;
		while (std::getline(idStream, token, '-')) {
			ids.push_back(std::stoi(token));
		}

		// Check each entity and collect missing IDs
		std::vector<int> missingIds;
		for (int id : ids) {
			if (!manager.hasEntity(id)) {
				missingIds.push_back(id);
			}
		}

		if (!missingIds.empty()) {
			std::cerr << "Error in line: " << line << std::endl;
			std::cerr << "Missing entities with IDs: ";
			for (int missingId : missingIds) {
				std::cerr << missingId << " ";
			}
			std::cerr << std::endl;
			continue; // Skip to next line
		}

		auto& pathLinker = manager.addEntity<Empty>();
		auto& plc = pathLinker.addComponent<PathLinkerComponent>();

		// parse optional attributes
		if (colorPart.size() == 7 && colorPart[0] == '#') {
			int r = std::stoi(colorPart.substr(1, 2), nullptr, 16);
			int g = std::stoi(colorPart.substr(3, 2), nullptr, 16);
			int b = std::stoi(colorPart.substr(5, 2), nullptr, 16);
			plc.color = TazColor(r, g, b, 255);
		}

		plc.width = width;

		for (size_t i = 1; i < ids.size(); ++i) {
			int idA = ids[i - 1];
			int idB = ids[i];

			auto& link = manager.addEntity<Link>(idA, idB);

			link.addGroup(Manager::groupPathLinks);

			addLinkFunc(link);

			pathLinker.GetComponent<PathLinkerComponent>().addLink(&link);
			pathLinker.addGroup(Manager::groupPathLinksHolder);

			linkEntities.push_back(&link);
		}
	}

	for (auto& link : manager.getGroup<LinkEntity>(Manager::groupPathLinks)) {
		manager.grid->addLink(link, manager.grid->getGridLevel());
	}
	manager.updateInnerPathLinks = true;

}