#include "./TextMapParser.h"

TextMapParser::TextMapParser(std::ifstream& file) : file(file) {}

void TextMapParser::parse(Manager& manager,
	std::function<void(Entity&, glm::vec3)> addNodeFunc,
	std::function<void(Entity&)> addLinkFunc) 
{
	std::string line;
	std::getline(file, line); // for first line

	glm::vec2 minPos(FLT_MAX);
	glm::vec2 maxPos(FLT_MIN);
	while (std::getline(file, line) && !line.empty()) {
		std::istringstream nodeLine(line);
		int id;
		float x, y, z;

		nodeLine >> id;
		nodeLine >> x >> y >> z;

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
		auto& node(manager.addEntity<Node>());
		addNodeFunc(node, glm::vec3(x, y, z));

		node.GetComponent<TransformComponent>().temp_lineParsed = line;
	}

	std::getline(file, line);

	while (std::getline(file, line)) {
		std::istringstream linkLine(line);
		unsigned int id, fromNodeId, toNodeId;

		linkLine >> id >> fromNodeId >> toNodeId;

		auto& link(manager.addEntity<Link>(fromNodeId, toNodeId));
		addLinkFunc(link);
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

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	main_camera2D->setPosition_X((maxPos.x + minPos.x) / 2.0f);
	main_camera2D->setPosition_Y((maxPos.y + minPos.y) / 2.0f);

	float aspect = static_cast<float>(main_camera2D->getCameraDimensions().x) /
		static_cast<float>(main_camera2D->getCameraDimensions().y);

	float zFromWidth = width / 2.0f / (std::tan(glm::radians(45.0f) / 2.0f) * aspect);

	float zFromHeight = height / 2.0f / (std::tan(glm::radians(45.0f) / 2.0f) * aspect);

#if defined(_WIN32) || defined(_WIN64)
	float requiredZ = std::max(zFromHeight, zFromWidth);
#else
	float requiredZ = std::max(zFromHeight, zFromWidth);

#endif

	main_camera2D->setPosition_Z(-requiredZ);

	main_camera2D->setAimPos(glm::vec3(main_camera2D->eyePos.x, main_camera2D->eyePos.y, main_camera2D->eyePos.z + 1.0f));

}