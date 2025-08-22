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
        std::string token;
        std::vector<int> ids;

        auto& pathLinker = manager.addEntity<Empty>();
        pathLinker.addComponent<PathLinkerComponent>();

        while (std::getline(ss, token, '-')) {
            ids.push_back(std::stoi(token));
        }

        for (size_t i = 1; i < ids.size(); ++i) {
            int idA = ids[i - 1];
            int idB = ids[i];

            auto& link = manager.addEntity<Link>(idA, idB);

            link.addGroup(Manager::groupPathLinks_0);

            pathLinker.GetComponent<PathLinkerComponent>().addLink(&link);
            pathLinker.addGroup(Manager::groupPathLinksHolder);

            linkEntities.push_back(&link);
        }
    }

    for (int i = 0; i < linkEntities.size(); i++) {
        addLinkFunc(*linkEntities[i]);
    }

    for (auto& link : manager.getGroup<LinkEntity>(Manager::groupPathLinks_0)) {
        manager.grid->addLink(link, manager.grid->getGridLevel());
    }
    manager.updateInnerPathLinks = true;

    //for (auto& link : manager.getGroup<LinkEntity>(Manager::groupPathLinks_1)) {
    //    manager.grid->addLink(link, manager.grid->getGridLevel());
    //}
    //for (auto& link : manager.getGroup<LinkEntity>(Manager::groupPathLinks_2)) {
    //    manager.grid->addLink(link, manager.grid->getGridLevel());
    //}
}