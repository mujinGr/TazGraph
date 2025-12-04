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

	for (auto* entity : nodes) {
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

	for (auto* link : links) {
		auto* entity = dynamic_cast<LinkEntity*>(link);

		auto fromNode = entity->getFromNode();
		auto toNode = entity->getToNode();

		UInt32 fromIdx = nodeIdMap[fromNode];
		UInt32 toIdx = nodeIdMap[toNode];

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
	std::function<void(Entity&, glm::vec3, std::string)> addNodeFunc,
	std::function<void(Entity&, std::string)> addLinkFunc)
{
	sim_dump::FileReader reader(fileName);
	assert(reader.is_file_valid());

	// --- Collect nodes ---
	glm::vec2 minPos(FLT_MAX);
	glm::vec2 maxPos(FLT_MIN);

	for (auto it = reader.get_node_iterator(); it != reader.get_node_end(); ++it) {
		float x = it->x;
		float y = it->y;

		parsedNodes.push_back({
			glm::vec3(x, y, 0.0f),
			TazColor(it->data.color.r,it->data.color.g,it->data.color.b,it->data.color.alpha),
			glm::vec3(it->data.dimension) });

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

		parsedLinks.push_back(
			SimulationStep::TazSimulationLink(
				TazColor(it->data.color.r, it->data.color.g, it->data.color.b, it->data.color.alpha),
				it->data.dimension
			)
		);
	}

	//  === INITIAL CREATION OF NODES ===


	createSteps(reader, manager, addNodeFunc, addLinkFunc);


	// --- Grid setup ---
	float width = maxPos.x - minPos.x;
	float height = maxPos.y - minPos.y;
	float maxDistance = std::max(width, height);

	manager.grid->setSize(2 * maxDistance);

	for (auto& node : DataManager::getInstance().mapSimToGraphNodes) {
		manager.grid->addNode(node.second, manager.grid->getGridLevel());
	}

	for (auto& link : DataManager::getInstance().mapSimToGraphLinks) {
		manager.grid->addLink(link.second, manager.grid->getGridLevel());
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
	std::function<void(Entity&, glm::vec3, std::string)> addNodeFunc,
	std::function<void(Entity&, std::string)> addLinkFunc) {



	// Create all nodes ONCE before the loop
	int i = 0;

	for (auto it = reader.get_node_iterator(); it != reader.get_node_end(); ++it) {
		auto& node = manager.addEntityWithId<Node>(i);
		node.addGroup(Manager::groupNodes_0);
		DataManager::getInstance().mapSimToGraphNodes[it->data.id] = &node;
		i++;
	}
	// Create all links ONCE before the loop  
	for (auto it = reader.get_link_iterator(); it != reader.get_link_end(); ++it) {
		auto& link = manager.addEntityWithId<Link>(i, (int)it->src_id, (int)it->dst_id);
		link.addGroup(Manager::groupLinks_0);

		DataManager::getInstance().mapSimToGraphLinks[it->data.id] = &link;
		i++;
	}

	auto& nodeEntities = DataManager::getInstance().mapSimToGraphNodes;
	auto& linkEntities = DataManager::getInstance().mapSimToGraphLinks;
	auto& pathEntities = DataManager::getInstance().mapSimToGraphPaths;

	std::vector<int> nodeList;
	nodeList.reserve(nodeEntities.size());
	std::vector<int> linkList;
	linkList.reserve(linkEntities.size());
	std::vector<int> pathList;
	pathList.reserve(pathEntities.size());

	for (auto& kv : nodeEntities) {
		nodeList.push_back(kv.first); // kv.second is Node*
	}
	for (auto& kv : linkEntities) {
		linkList.push_back(kv.first); // kv.second is Node*
	}
	for (auto& kv : pathEntities) {
		pathList.push_back(kv.first); // kv.second is Node*
	}

	if (_threader) {
		_threader->parallel(nodeList.size(), [&](int start, int end) {
			for (int i = start; i < end; i++) {
				int key = nodeList[i];
				std::string id_string = reader.get_entity_data_string(sim_dump::EntityType::NODE, key);

				nodeEntities[key]->addComponent<TransformComponent>(parsedNodes[i].position, parsedNodes[i].size, 1);
				nodeEntities[key]->addComponent<Rectangle_w_Color>();
				nodeEntities[key]->GetComponent<Rectangle_w_Color>().setColor(parsedNodes[i].color);
				addNodeFunc(*nodeEntities[key], glm::vec3(0), id_string);
			}
			});
	}

	if (_threader) {
		_threader->parallel(linkEntities.size(), [&](int start, int end) {
			for (int i = start; i < end; i++) {
				int key = linkList[i];
				std::string id_string = reader.get_entity_data_string(sim_dump::EntityType::LINK, key);

				auto& lwc = linkEntities[key]->addComponent<Line_w_Color>();
				lwc.setSrcColor(parsedLinks[i].color);
				lwc.setDestColor(parsedLinks[i].color);
				lwc.width = parsedLinks[i].width;
				addLinkFunc(*linkEntities[key], id_string);
			}
			});
	}
	manager.setNewLastEntityId();
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

		for (UInt32 i = 0; i < reader.get_link_count(); i++) {
			auto color = reader.get_entity_color(EntityType::LINK, i);
			float width = reader.get_entity_width(EntityType::LINK, i);

			step.links[i].second.color = TazColor(color.r, color.g, color.b, color.alpha);
			step.links[i].second.width = width;

			step.links[i].first = linkEntities[i];
		}

		step.paths.resize(reader.get_path_count());
		// paths
		UInt32 i = 0;
		for (auto it = reader.get_path_iterator();
			it != reader.get_path_end(); ++it)
		{
			auto color = it->second.data.color;
			float width = it->second.data.dimension;
			int id = it->first;

			EmptyEntity* pathEntity = nullptr;

			// Check if path already exists
			if (pathEntities[id]) {
				pathEntity = pathEntities[id];
			}
			else {
				// Create a new one only if missing
				auto& empty_pathHolder = manager.addEntity<Empty>();
				empty_pathHolder.addComponent<PathLinkerComponent>();
				//empty_pathHolder.addGroup(Manager::groupPathLinksHolder);
				pathEntities[id] = &empty_pathHolder;
				pathEntity = &empty_pathHolder;
			}

			step.paths[i].first = pathEntity;

			std::vector<EntityID> path_linkIds;

			for (auto& pathIt : it->second.links) {
				path_linkIds.push_back(linkEntities[pathIt]->getId());
			}

			step.paths[i].second = {
				TazColor(color.r, color.g, color.b, color.alpha),
				width,
				path_linkIds
			};
			i++;
		}
		manager.steps.push_back(std::move(step));
	} while (reader.next());
}