#include "./TextMapParser.h"

TextMapParser::TextMapParser() {}

void TextMapParser::readFile(std::string m_fileName) {
	file.open(m_fileName);

	if (!file.is_open()) {
		std::cerr << "Failed to open file for reading: " << m_fileName << std::endl;
		return;
	}
}

void TextMapParser::writeFile(std::string m_fileName, Manager& manager)
{

	auto& nodes(manager.getGroup<NodeEntity>(Manager::groupNodes_0));
	auto& links(manager.getGroup<LinkEntity>(Manager::groupLinks_0));

	std::ofstream file(m_fileName);

	if (!file.is_open()) {
		std::cerr << "Failed to open file for writing: " << m_fileName << std::endl;
		return;
	}

	file << "Total number of nodes: " << nodes.size() << "\n";

	for (auto* entity : nodes) {
		NodeEntity* node_entity = dynamic_cast<NodeEntity*>(entity);

		if (entity->hasComponent<TransformComponent>()) {
			TransformComponent& tc = entity->GetComponent<TransformComponent>();
			file << EntityIDUtils::toString(entity->getId()) << "\t"; // id is the index in the vector of entities
			file << tc.getPosition().x << " " << tc.getPosition().y << "\t";
			file << tc.size.x << "x" << tc.size.y << "\n";
		}
	}

	file << "\n";

	file << "Total number of links: " << links.size() << "\n";

	for (auto* entity : links) {
		LinkEntity* link_entity = dynamic_cast<LinkEntity*>(entity);
		file << EntityIDUtils::toString(link_entity->getId()) << "\t";
		file << EntityIDUtils::toString(link_entity->getFromNode()) << "\t";
		file << EntityIDUtils::toString(link_entity->getToNode()) << "\n";
	}

	file.close();
}

void TextMapParser::closeFile() {
	file.close();
}

void TextMapParser::update(float deltaTime)
{
}

void TextMapParser::parse(Manager& manager,
	std::function<void(Entity&, glm::vec3)> addNodeFunc,
	std::function<void(Entity&)> addLinkFunc)
{
	std::string line;
	std::getline(file, line); // for first line

	std::vector<std::string> nodeLines;
	while (std::getline(file, line) && !line.empty()) {
		nodeLines.push_back(line);
	}

	std::getline(file, line); // skip middle line

	// Read links
	std::vector<std::string> linkLines;
	while (std::getline(file, line)) {
		linkLines.push_back(line);
	}

	std::vector<ParsedNode> parsedNodes(nodeLines.size());

	struct MinMax {
		glm::vec2 min;
		glm::vec2 max;
	};

	std::vector<MinMax> localExtremes(_threader->num_threads,
		{ glm::vec2(FLT_MAX), glm::vec2(FLT_MIN) });

	glm::vec2 minPos(FLT_MAX);
	glm::vec2 maxPos(FLT_MIN);

	if (_threader) {
		_threader->parallel(nodeLines.size(), [&](int start, int end) {
			glm::vec2 local_minPos(FLT_MAX);
			glm::vec2 local_maxPos(FLT_MIN);

			for (int i = start; i < end; i++) {
				std::istringstream nodeLine(nodeLines[i]);
				int id;
				float x, y;
				nodeLine >> id >> x >> y;

				parsedNodes[i] = { id, glm::vec3(x, y, 0) };

				// Update local min/max
				local_minPos.x = std::min(local_minPos.x, x);
				local_minPos.y = std::min(local_minPos.y, y);
				local_maxPos.x = std::max(local_maxPos.x, x);
				local_maxPos.y = std::max(local_maxPos.y, y);
			}
			int threadID = (start * _threader->num_threads) / nodeLines.size();
			localExtremes[threadID] = { local_minPos, local_maxPos };
			});
	}

	for (const auto& mm : localExtremes) {
		minPos.x = std::min(minPos.x, mm.min.x);
		minPos.y = std::min(minPos.y, mm.min.y);
		maxPos.x = std::max(maxPos.x, mm.max.x);
		maxPos.y = std::max(maxPos.y, mm.max.y);
	}

	std::vector<ParsedLink> parsedLinks(linkLines.size());

	if (_threader) {
		_threader->parallel(linkLines.size(), [&](int start, int end) {
			for (int i = start; i < end; i++) {
				std::istringstream linkLine(linkLines[i]);
				int id, fromNodeId, toNodeId;
				linkLine >> id >> fromNodeId >> toNodeId;

				parsedLinks[i] = { id, fromNodeId, toNodeId };
			}
			});
	}

	std::vector<Entity*> nodeEntities;
	nodeEntities.reserve(parsedNodes.size());

	for (const auto& parsedNode : parsedNodes) {
		auto& node(manager.addEntity<Node>());

		node.addToGroup(Manager::groupNodes_0);

		//addNodeFunc(node, glm::vec3(x, y, z));
		nodeEntities.push_back(&node);
	}

	if (_threader) {
		_threader->parallel(nodeEntities.size(), [&](int start, int end) {
			for (int i = start; i < end; i++) {
				addNodeFunc(*nodeEntities[i], parsedNodes[i].pos);
			}
			});
	}

	std::vector<Entity*> linkEntities;
	linkEntities.reserve(parsedLinks.size());

	for (const auto& parsedLink : parsedLinks) {
		auto& link = manager.addEntity<Link>(parsedLink.fromId, parsedLink.toId);

		link.addToGroup(Manager::groupLinks_0);

		linkEntities.push_back(&link);
	}

	if (_threader) {
		_threader->parallel(linkEntities.size(), [&](int start, int end) {
			for (int i = start; i < end; i++) {
				addLinkFunc(*linkEntities[i]);
			}
			});
	}

	float width = maxPos.x - minPos.x;
	float height = maxPos.y - minPos.y;

	float maxDistance = width > height ? width : height;

	//! Set grid size
	manager.grid->setSize(2 * maxDistance);

	for (auto* node : manager.getGroup<NodeEntity>(Manager::groupNodes_0)) {
		NodeEntity* node_entity = dynamic_cast<NodeEntity*>(node);

		manager.grid->addNode(node_entity, manager.grid->getGridLevel());
	}

	for (auto link : manager.getGroup<LinkEntity>(Manager::groupLinks_0)) {
		LinkEntity* link_entity = dynamic_cast<LinkEntity*>(link);

		manager.grid->addLink(link_entity, manager.grid->getGridLevel());
	}

	//! Set camera based on map loaded
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	main_camera2D->setPosition_X((maxPos.x + minPos.x) / 2.0f);
	main_camera2D->setPosition_Y((maxPos.y + minPos.y) / 2.0f);

	float aspect = static_cast<float>(main_camera2D->getCameraDimensions().x) /
		static_cast<float>(main_camera2D->getCameraDimensions().y);

	float zFromWidth = width / 2.0f / (std::tan(glm::radians(45.0f) / 2.0f) * aspect);

	float zFromHeight = height / 2.0f / (std::tan(glm::radians(45.0f) / 2.0f) * aspect);

	float requiredZ = std::max({ 1000.0f,  zFromHeight, zFromWidth });

	main_camera2D->setPosition_Z(-requiredZ);

	main_camera2D->setAimPos(glm::vec3(main_camera2D->eyePos.x, main_camera2D->eyePos.y, main_camera2D->eyePos.z + 1.0f));
}