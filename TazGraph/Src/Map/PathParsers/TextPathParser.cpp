#include "./TextPathParser.h"

#ifdef _WIN32 || _WIN64
#define safe_sscanf sscanf_s
#else
#define safe_sscanf sscanf
#endif

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
		auto& plc = pathLinker.addComponent<PathLinkerComponent>();

        while (std::getline(ss, token, '-')) {
            ids.push_back(std::stoi(token));
        }

		// parse optional attributes
		std::string attr;
		while (std::getline(ss, attr, ';')) {
			std::stringstream attrSS(attr);
			std::string key, value;
			if (std::getline(attrSS, key, '=')) {
				if (std::getline(attrSS, value)) {
					if (key.find("width") != std::string::npos) {
						plc.width = std::stof(value);
					}
					else if (key.find("color") != std::string::npos) {
						int r, g, b;
						if (safe_sscanf(value.c_str(), "%d,%d,%d", &r, &g, &b) == 3) {
							plc.color = Color(r, g, b, 255);
						}
					}
				}
			}
		}

        for (size_t i = 1; i < ids.size(); ++i) {
            int idA = ids[i - 1];
            int idB = ids[i];

            auto& link = manager.addEntity<Link>(idA, idB);

            link.addGroup(Manager::groupPathLinks_0);

			addLinkFunc(link);
            
			pathLinker.GetComponent<PathLinkerComponent>().addLink(&link);
            pathLinker.addGroup(Manager::groupPathLinksHolder);

            linkEntities.push_back(&link);
        }
    }

    for (auto& link : manager.getGroup<LinkEntity>(Manager::groupPathLinks_0)) {
        manager.grid->addLink(link, manager.grid->getGridLevel());
    }
    manager.updateInnerPathLinks = true;

}