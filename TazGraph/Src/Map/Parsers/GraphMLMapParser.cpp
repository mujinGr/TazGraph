#include "./GraphMLMapParser.h"

GraphMLMapParser::GraphMLMapParser() {}

void GraphMLMapParser::readFile(std::string m_fileName) {
	file = doc.LoadFile(m_fileName.c_str());

	if (file != tinyxml2::XML_SUCCESS) {
		std::cerr << "Error loading GraphML file: " << doc.ErrorStr() << std::endl;
		return;
	}
}

void GraphMLMapParser::closeFile() {

}

void GraphMLMapParser::parse(Manager& manager,
	std::function<void(Entity&, glm::vec3)> addNodeFunc,
	std::function<void(Entity&)> addLinkFunc)
{

	// Store node positions for later edge processing
	std::vector<glm::vec3> nodePositions;

	// 1. Parse nodes
	tinyxml2::XMLElement* graph = doc.FirstChildElement("graphml")->FirstChildElement("graph");
	for (tinyxml2::XMLElement* node = graph->FirstChildElement("node");
		node != nullptr;
		node = node->NextSiblingElement("node"))
	{
		int id = std::stoi(node->Attribute("id"));

		// Extract position from data (assuming x,y,z properties)
		float x = 0, y = 0;
		for (tinyxml2::XMLElement* data = node->FirstChildElement("data");
			data != nullptr;
			data = data->NextSiblingElement("data"))
		{
			const char* key = data->Attribute("key");
			if (strcmp(key, "x") == 0) x = std::stof(data->GetText());
			else if (strcmp(key, "y") == 0) y = std::stof(data->GetText());
		}

		// Create node entity
		auto& entity = manager.addEntity<Node>();

		entity.addGroup(Manager::groupNodes_0);

		glm::vec3 position(x, y, 0);
		addNodeFunc(entity, position);

		// Store position for edge processing
		nodePositions.resize(nodePositions.size() + 1);
		nodePositions[id] = position;
	}

	// 2. Parse edges
	for (tinyxml2::XMLElement* edge = graph->FirstChildElement("edge");
		edge != nullptr;
		edge = edge->NextSiblingElement("edge"))
	{
		int sourceId = std::stoi(edge->Attribute("source"));
		int targetId = std::stoi(edge->Attribute("target"));

		// Create link entity (assuming you have a way to reference nodes by ID)
		auto& link = manager.addEntity<Link>(sourceId, targetId);

		link.addGroup(Manager::groupLinks_0);

		addLinkFunc(link);

		// Optional: Set edge properties
		for (tinyxml2::XMLElement* data = edge->FirstChildElement("data");
			data != nullptr;
			data = data->NextSiblingElement("data"))
		{
			const char* key = data->Attribute("key");
			// Handle edge properties if needed
		}
	}

	
	for (auto& node : manager.getGroup<NodeEntity>(Manager::groupNodes_0)) {
		manager.grid->addNode(node, manager.grid->getGridLevel());
	}

	for (auto& link : manager.getGroup<LinkEntity>(Manager::groupLinks_0)) {
		manager.grid->addLink(link, manager.grid->getGridLevel());
	}

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	main_camera2D->setAimPos(glm::vec3(main_camera2D->eyePos.x, main_camera2D->eyePos.y, main_camera2D->eyePos.z + 1.0f));

}