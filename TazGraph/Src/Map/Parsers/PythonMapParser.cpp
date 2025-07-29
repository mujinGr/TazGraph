#include "./PythonMapParser.h"


PythonMapParser::PythonMapParser(std::ifstream& file) : file(file) {}

void PythonMapParser::parse(Manager& manager,
	std::function<void(Entity&, glm::vec3)> addNodeFunc,
	std::function<void(Entity&)> addLinkFunc) 
{

	glm::vec2 minPos(FLT_MAX);
	glm::vec2 maxPos(FLT_MIN);

	JsonParser fileParser(file);
	JsonValue rootFromFile = fileParser.parse();
	auto& nodes = rootFromFile.obj["graph"].obj["nodes"];

	for (auto& nodeEntry : nodes.obj) {
		int nodeId = std::stoi(nodeEntry.first);
		auto& nodeInfo = nodeEntry.second;
		float x = nodeInfo.obj["metadata"].obj["x"].num / 10.0f;
		float y = nodeInfo.obj["metadata"].obj["y"].num / 10.0f;

		auto& node = manager.addEntity<Node>();
		glm::vec3 position(x, y, 0);

#if defined(_WIN32) || defined(_WIN64)
		minPos.x = std::min(minPos.x, x);
		minPos.y = std::min(minPos.y, y);
		maxPos.x = std::max(maxPos.x, x);
		maxPos.y = std::max(maxPos.y, y);
#else
		minPos.x = std::min(minPos.x, x);
		minPos.y = std::min(minPos.y, y);
		maxPos.x = std::max(maxPos.x, x);
		maxPos.y = std::max(maxPos.y, y);
#endif

		addNodeFunc(node, position);

		node.GetComponent<TransformComponent>().temp_lineParsed = nodeId;

	}
	auto& links = rootFromFile.obj["graph"].obj["edges"];
	for (auto& linkEntry : links.arr) {
		auto& linkInfo = linkEntry;
		unsigned int fromID = linkInfo.obj["source"].num;
		unsigned int toID = linkInfo.obj["target"].num;

		auto& link = manager.addEntity<Link>(fromID, toID);
		addLinkFunc(link);

		link.GetComponent<Line_w_Color>().temp_lineParsed = std::to_string(fromID) + " - " + std::to_string(toID);

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