#include "./SimDumpMapParser.h"

SimDumpMapParser::SimDumpMapParser() {}

void SimDumpMapParser::readFile(std::string m_fileName) {
	file.open(m_fileName);
	fileName = m_fileName;

	if (!file.is_open()) {
		std::cerr << "Failed to open file for reading: " << m_fileName << std::endl;
		return;
	}
}

void SimDumpMapParser::closeFile() {
	file.close();
}

void SimDumpMapParser::parse(Manager& manager,
	std::function<void(Entity&, glm::vec3)> addNodeFunc,
	std::function<void(Entity&)> addLinkFunc)
{
	sim_dump::FileReader reader(fileName);
	assert(reader.is_file_valid());

	// --- Collect nodes ---
	std::vector<ParsedNode> parsedNodes;
	glm::vec2 minPos(FLT_MAX);
	glm::vec2 maxPos(FLT_MIN);

	for (auto it = reader.get_node_iterator(); it != reader.get_node_end(); ++it) {
		float x = it->x / 1000.0f;
		float y = it->y / 1000.0f;

		parsedNodes.push_back({ 0, glm::vec3(x, y, 0.0f) });

		// Track global min/max
		minPos.x = std::min(minPos.x, x);
		minPos.y = std::min(minPos.y, y);
		maxPos.x = std::max(maxPos.x, x);
		maxPos.y = std::max(maxPos.y, y);
	}

	// --- Collect links ---
	std::vector<ParsedLink> parsedLinks;
	for (auto it = reader.get_link_iterator(); it != reader.get_link_end(); ++it) {
		int fromNode = it->src_id;
		int toNode = it->dst_id;

		parsedLinks.push_back({ 0, fromNode, toNode });
	}

	// --- Create node entities ---
	std::vector<Entity*> nodeEntities;
	nodeEntities.reserve(parsedNodes.size());

	for (const auto& parsedNode : parsedNodes) {
		auto& node = manager.addEntity<Node>();
		node.addGroup(Manager::groupNodes_0);
		nodeEntities.push_back(&node);
	}

	if (_threader) {
		_threader->parallel(nodeEntities.size(), [&](int start, int end) {
			for (int i = start; i < end; i++) {
				addNodeFunc(*nodeEntities[i], parsedNodes[i].pos);
			}
			});
	}

	// --- Create link entities ---
	std::vector<Entity*> linkEntities;
	linkEntities.reserve(parsedLinks.size());

	for (const auto& parsedLink : parsedLinks) {
		auto& link = manager.addEntity<Link>(parsedLink.fromId, parsedLink.toId);
		link.addGroup(Manager::groupLinks_0);
		linkEntities.push_back(&link);
	}

	if (_threader) {
		_threader->parallel(linkEntities.size(), [&](int start, int end) {
			for (int i = start; i < end; i++) {
				addLinkFunc(*linkEntities[i]);
			}
			});
	}

	// --- Grid setup ---
	float width = maxPos.x - minPos.x;
	float height = maxPos.y - minPos.y;
	float maxDistance = std::max(width, height);

	manager.grid->setSize(2 * maxDistance);

	for (auto& node : manager.getGroup<NodeEntity>(Manager::groupNodes_0)) {
		manager.grid->addNode(node, manager.grid->getGridLevel());
	}

	for (auto& link : manager.getGroup<LinkEntity>(Manager::groupLinks_0)) {
		manager.grid->addLink(link, manager.grid->getGridLevel());
	}

	// --- Camera setup ---
	std::shared_ptr<PerspectiveCamera> main_camera2D =
		std::dynamic_pointer_cast<PerspectiveCamera>(
			CameraManager::getInstance().getCamera("main"));

	main_camera2D->setPosition_X((maxPos.x + minPos.x) / 2.0f);
	main_camera2D->setPosition_Y((maxPos.y + minPos.y) / 2.0f);

	float aspect = static_cast<float>(main_camera2D->getCameraDimensions().x) /
		static_cast<float>(main_camera2D->getCameraDimensions().y);

	float zFromWidth = width / 2.0f / (std::tan(glm::radians(45.0f) / 2.0f) * aspect);
	float zFromHeight = height / 2.0f / (std::tan(glm::radians(45.0f) / 2.0f) * aspect);

	float requiredZ = std::max(zFromHeight, zFromWidth);
	main_camera2D->setPosition_Z(-requiredZ);

	main_camera2D->setAimPos(glm::vec3(
		main_camera2D->eyePos.x,
		main_camera2D->eyePos.y,
		main_camera2D->eyePos.z + 1.0f
	));
}