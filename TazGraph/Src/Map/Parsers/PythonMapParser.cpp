#include "./PythonMapParser.h"


PythonMapParser::PythonMapParser(std::ifstream& file) : file(file) {}

void PythonMapParser::parse(Manager& manager,
	std::function<void(Entity&, glm::vec3)> addNodeFunc,
	std::function<void(Entity&)> addLinkFunc) 
{

	JsonParser fileParser(file);
	JsonValue rootFromFile = fileParser.parse();
	auto& nodes = rootFromFile.obj["graph"].obj["nodes"];
	auto& links = rootFromFile.obj["graph"].obj["edges"];

	std::vector<std::pair<std::string, JsonValue*>> nodeEntries;
	nodeEntries.reserve(nodes.obj.size());
	for (auto& nodeEntry : nodes.obj) {
		nodeEntries.emplace_back(nodeEntry.first, &nodeEntry.second);
	}

	std::vector<JsonValue*> linkEntries;
	linkEntries.reserve(links.arr.size());
	for (auto& linkEntry : links.arr) {
		linkEntries.push_back(&linkEntry);
	}

	std::vector<ParsedNode> parsedNodes(nodeEntries.size());

	struct MinMax {
		glm::vec2 min;
		glm::vec2 max;
	};

	std::vector<MinMax> localExtremes(_threader->num_threads);

	glm::vec2 minPos(FLT_MAX);
	glm::vec2 maxPos(FLT_MIN);

	if (_threader) {
		_threader->parallel(nodeEntries.size(), [&](int start, int end) {
			glm::vec2 local_minPos(FLT_MAX);
			glm::vec2 local_maxPos(FLT_MIN);
			
			for (int i = start; i < end; i++) {
				int nodeId = std::stoi(nodeEntries[i].first);
				auto& nodeInfo = *nodeEntries[i].second;
				float x = nodeInfo.obj["metadata"].obj["x"].num / 10.0f;
				float y = nodeInfo.obj["metadata"].obj["y"].num / 10.0f;

				parsedNodes[i] = { nodeId, glm::vec3(x, y, 0) };

				local_minPos.x = std::min(local_minPos.x, x);
				local_minPos.y = std::min(local_minPos.y, y);
				local_maxPos.x = std::max(local_maxPos.x, x);
				local_maxPos.y = std::max(local_maxPos.y, y);
			}
			int threadID = (start * _threader->num_threads) / nodeEntries.size();
			localExtremes[threadID] = { local_minPos, local_maxPos };
			});
	}

	for (const auto& mm : localExtremes) {
		minPos.x = std::min(minPos.x, mm.min.x);
		minPos.y = std::min(minPos.y, mm.min.y);
		maxPos.x = std::max(maxPos.x, mm.max.x);
		maxPos.y = std::max(maxPos.y, mm.max.y);
	}

	std::vector<ParsedLink> parsedLinks(linkEntries.size());

	if (_threader) {
		_threader->parallel(linkEntries.size(), [&](int start, int end) {
			for (int i = start; i < end; i++) {
				auto& linkInfo = *linkEntries[i];
				int fromID = linkInfo.obj["source"].num;
				int toID = linkInfo.obj["target"].num;

				parsedLinks[i] = {i, fromID, toID };
			}
			});
	}

	std::vector<Entity*> nodeEntities;
	nodeEntities.reserve(parsedNodes.size());

	for (const auto& parsedNode : parsedNodes) {
		auto& node(manager.addEntity<Node>());

		node.addGroup(Manager::groupNodes_0);

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

	float width = maxPos.x - minPos.x;
	float height = maxPos.y - minPos.y;

	float maxDistance = width > height ? width : height;

	manager.grid->setSize(2 * maxDistance);

	for (auto& node : manager.getGroup<NodeEntity>(Manager::groupNodes_0)) {
		manager.grid->addNode(node, manager.grid->getGridLevel());
	}

	for (auto& link : manager.getGroup<LinkEntity>(Manager::groupLinks_0)) {
		manager.grid->addLink(link, manager.grid->getGridLevel());
	}


	std::cout << "Parsed JSON from file successfully!" << std::endl;

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	main_camera2D->setPosition_X((maxPos.x + minPos.x) / 2.0f);
	main_camera2D->setPosition_Y((maxPos.y + minPos.y) / 2.0f);

	float aspect = static_cast<float>(main_camera2D->getCameraDimensions().x) /
		static_cast<float>(main_camera2D->getCameraDimensions().y);

	float zFromWidth = (maxPos.x - minPos.x) / 2.0f / (std::tan(glm::radians(45.0f) / 2.0f) * aspect);

	float zFromHeight = (maxPos.y - minPos.y) / 2.0f / (std::tan(glm::radians(45.0f) / 2.0f) * aspect);

#if defined(_WIN32) || defined(_WIN64)
	float requiredZ = std::max(zFromHeight, zFromWidth);
#else
	float requiredZ = std::max(zFromHeight, zFromWidth);

#endif

	main_camera2D->setPosition_Z(-requiredZ);

	main_camera2D->setAimPos(glm::vec3(main_camera2D->eyePos.x, main_camera2D->eyePos.y, main_camera2D->eyePos.z + 1.0f));
}