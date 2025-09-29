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

void SimDumpMapParser::writeFile(std::string m_fileName, Manager& manager)
{
	sim_dump::FileWriter writer(m_fileName);

	// ===== Write Nodes =====
	auto& nodes(manager.getGroup<NodeEntity>(Manager::groupNodes_0));
	std::unordered_map<EntityID, UInt32> nodeIdMap; // map ECS ID -> writer ID
	nodeIdMap.reserve(nodes.size());

	for (auto& entity : nodes) {
		if (!entity->hasComponent<TransformComponent>())
			continue;

		auto& tc = entity->GetComponent<TransformComponent>();
		glm::vec3 pos = tc.getPosition();

		// Scale could be used as width/size, or default to 1.0f
		float width = tc.size.x;

		// For now assign a default color, or pull from a ColorComponent if you have one
		Color color(200, 200, 200, 255);

		UInt32 nodeIdx = writer.add_node(pos.x, pos.y, width, color);
		nodeIdMap[entity->getId()] = nodeIdx;
	}

	// ===== Write Links =====
	auto& links(manager.getGroup<LinkEntity>(Manager::groupLinks_0));
	std::unordered_map<EntityID, UInt32> linkIdMap;
	linkIdMap.reserve(links.size());

	for (auto& entity : links) {
		auto* fromNode = entity->getFromNode();
		auto* toNode = entity->getToNode();
		if (!fromNode || !toNode) continue;

		UInt32 fromIdx = nodeIdMap[fromNode->getId()];
		UInt32 toIdx = nodeIdMap[toNode->getId()];

		// Default width & color, or pull from components
		float width = 1.0f;
		Color color(128, 128, 128, 255);

		UInt32 linkIdx = writer.add_link(fromIdx, toIdx, width, color);
		linkIdMap[entity->getId()] = linkIdx;
	}

	// ===== First step =====
	writer.advance_step(1.0); // timestamp = 1.0 (or 0.0 if you want "initial")

	// Optional: if you want to serialize paths or dynamic updates, do it here.
	// Example: add a single path covering all links
	if (!linkIdMap.empty()) {
		std::vector<UInt32> pathLinks;
		for (auto& [eid, lid] : linkIdMap) {
			pathLinks.push_back(lid);
		}
		writer.add_path(pathLinks, 2.0f, Color(255, 255, 0, 255));
	}

	// ===== Further steps (if you want incremental simulation changes) =====
	// Example: apply some dummy modifications
	writer.advance_step(2.0);
	if (!nodeIdMap.empty()) {
		auto firstNode = nodeIdMap.begin()->second;
		writer.change_node_position(firstNode, 5.0f, 5.0f);
		writer.change_color(EntityType::NODE, firstNode, Color(255, 0, 0, 255));
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
	glm::vec2 minPos(FLT_MAX);
	glm::vec2 maxPos(FLT_MIN);

	for (auto it = reader.get_node_iterator(); it != reader.get_node_end(); ++it) {
		float x = it->x;
		float y = it->y;

		parsedNodes.push_back({ 0, glm::vec3(x, y, 0.0f) });

		// Track global min/max
		minPos.x = std::min(minPos.x, x);
		minPos.y = std::min(minPos.y, y);
		maxPos.x = std::max(maxPos.x, x);
		maxPos.y = std::max(maxPos.y, y);
	}

	// --- Collect links ---
	for (auto it = reader.get_link_iterator(); it != reader.get_link_end(); ++it) {
		int fromNode = it->src_id;
		int toNode = it->dst_id;

		parsedLinks.push_back({ 0, fromNode, toNode });
	}

	//  === INITIAL CREATION OF NODES ===
	std::vector<NodeEntity*> nodeEntities;
	std::vector<LinkEntity*> linkEntities;


	createSteps(reader, manager, nodeEntities, linkEntities, addNodeFunc, addLinkFunc);


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

	float requiredZ = std::max({ 1000.0f,  zFromHeight, zFromWidth });
	main_camera2D->setPosition_Z(-requiredZ);

	main_camera2D->setAimPos(glm::vec3(
		main_camera2D->eyePos.x,
		main_camera2D->eyePos.y,
		main_camera2D->eyePos.z + 1.0f
	));
}

void SimDumpMapParser::createSteps(
	sim_dump::FileReader& reader,
	Manager& manager,
	std::vector<NodeEntity*>& nodeEntities,
	std::vector<LinkEntity*>& linkEntities,
	std::function<void(Entity&, glm::vec3)> addNodeFunc,
	std::function<void(Entity&)> addLinkFunc) {

	// Create all nodes ONCE before the loop
	for (UInt32 i = 0; i < reader.get_node_count(); i++) {
		auto& node = manager.addEntity<Node>();
		node.addGroup(Manager::groupNodes_0);
		nodeEntities.push_back(&node);
	}

	// Create all links ONCE before the loop  
	for (auto it = reader.get_link_iterator(); it != reader.get_link_end(); ++it) {
		auto& link = manager.addEntity<Link>((int)it->src_id, (int)it->dst_id);
		link.addGroup(Manager::groupLinks_0);
		linkEntities.push_back(&link);
	}

	if (_threader) {
		_threader->parallel(nodeEntities.size(), [&](int start, int end) {
			for (int i = start; i < end; i++) {
				addNodeFunc(*nodeEntities[i], parsedNodes[i].pos);
			}
			});
	}

	if (_threader) {
		_threader->parallel(linkEntities.size(), [&](int start, int end) {
			for (int i = start; i < end; i++) {
				addLinkFunc(*linkEntities[i]);
			}
			});
	}

	do {
		SimulationStep step;
		step.step_index = reader.get_current_step_index();
		step.timestamp = reader.get_current_timestamp();

		// nodes
		step.nodes.resize(reader.get_node_count());

		for (UInt32 i = 0; i < reader.get_node_count(); i++) {
			auto pos = reader.get_node_position(i);
			auto color = reader.get_entity_color(EntityType::NODE, i);
			float size = reader.get_entity_width(EntityType::NODE, i);

			step.nodes[i].second.position = glm::vec3(pos.first, pos.second, 0.0f);
			step.nodes[i].second.color = TazColor(color.r, color.g, color.b, color.alpha);
			step.nodes[i].second.size = glm::vec3(size);

			step.nodes[i].first = nodeEntities[i];
		}

		// links
		step.links.resize(reader.get_link_count());

		Uint32 i = 0;

		for (auto it = reader.get_link_iterator(); it != reader.get_link_end(); ++it) {
			auto color = reader.get_entity_color(EntityType::LINK, i);
			float width = reader.get_entity_width(EntityType::LINK, i);

			step.links[i].second.fromColor = TazColor(color.r, color.g, color.b, color.alpha);
			step.links[i].second.toColor = TazColor(color.r, color.g, color.b, color.alpha);
			step.links[i].second.width = width;

			step.links[i].first = linkEntities[i];

			i++;
		}

		// paths
		for (UInt32 i = 0; i < reader.get_path_count(); i++) {
			auto color = reader.get_entity_color(EntityType::PATH, i);
			float width = reader.get_entity_width(EntityType::PATH, i);

			step.paths[i] = {
				TazColor(color.r, color.g, color.b, color.alpha),
				width
			};
		}

		SimDumpPathParser pathParser;
		pathParser.parse(manager, reader,
			nodeEntities, 
			linkEntities,
			addNodeFunc, 
			addLinkFunc);

		manager.steps.push_back(std::move(step));
	} while (reader.next());
}