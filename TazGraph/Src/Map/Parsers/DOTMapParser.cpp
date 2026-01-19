#include "./DOTMapParser.h"

DOTMapParser::DOTMapParser() {}

void DOTMapParser::readFile(std::string m_fileName) {
	file.open(m_fileName);

}

void DOTMapParser::writeFile(std::string m_fileName, Manager& manager)
{
}

void DOTMapParser::closeFile() {
	file.close();
}

void DOTMapParser::update(float deltaTime)
{
}

void DOTMapParser::parse(Manager& manager,
	std::function<void(Entity&, glm::vec3)> addNodeFunc,
	std::function<void(Entity&)> addLinkFunc)
{

	std::ostringstream ss;
	ss << file.rdbuf(); // read whole file into a string
	std::string dotContent = ss.str();

	DOTParser::Graph parsedGraph = DOTParser::parseDot(dotContent);

	std::unordered_map<std::string, Entity*> nodeEntities;

	for (const auto& nodeIdStr : parsedGraph.nodes) {
		int nodeId = std::stoi(nodeIdStr.c_str());

		auto& entity = manager.addEntity<Node>();
		entity.setId(nodeId);

		entity.addToGroup(Manager::groupNodes_0);

		glm::vec3 position(0.0f);
		addNodeFunc(entity, position);

		nodeEntities[nodeIdStr] = &entity;
	}

	for (const auto& [sourceStr, targetStr] : parsedGraph.edges) {
		auto srcIt = nodeEntities.find(sourceStr);
		auto tgtIt = nodeEntities.find(targetStr);

		if (srcIt == nodeEntities.end() || tgtIt == nodeEntities.end()) {
			std::cerr << "Edge references missing node(s): "
				<< sourceStr << (parsedGraph.directed ? " -> " : " -- ")
				<< targetStr << std::endl;
			continue;
		}

		int fromId = std::stoi(sourceStr.c_str());
		int toId = std::stoi(targetStr.c_str());

		auto& link = manager.addEntity<Link>(fromId, toId);
		link.addToGroup(Manager::groupLinks_0);

		addLinkFunc(link);
	}


	for (auto* node : manager.getGroup<NodeEntity>(Manager::groupNodes_0)) {
		auto* node_entity = dynamic_cast<NodeEntity*>(node);
		manager.grid->addNode(node_entity, manager.grid->getGridLevel());
	}

	for (auto link : manager.getGroup<LinkEntity>(Manager::groupLinks_0)) {
		auto* link_entity = dynamic_cast<LinkEntity*>(link);

		manager.grid->addLink(link_entity, manager.grid->getGridLevel());
	}

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	main_camera2D->setAimPos(glm::vec3(main_camera2D->eyePos.x, main_camera2D->eyePos.y, main_camera2D->eyePos.z + 1.0f));

}