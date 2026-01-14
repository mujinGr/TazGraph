#include "PythonEngineUtil.h"

PythonEngineUtil::PythonEngineUtil()
{
}

void PythonEngineUtil::init(Manager& manager)
{
	py::exec(R"(
				import sys
				from io import StringIO
				sys.stdout = StringIO()
			)");

	py::module_ userapi = py::module_::create_extension_module("tazpyapi", nullptr, new PyModuleDef{});
	init_api(userapi, manager);
	py::module_::import("sys").attr("modules")["tazpyapi"] = userapi;

	py::globals()["addNode"] = userapi.attr("addNode");
	py::globals()["addLink"] = userapi.attr("addLink");
	py::globals()["getNodes"] = userapi.attr("getNodes");
	py::globals()["getCurrentStep"] = userapi.attr("getCurrentStep");
	py::globals()["getSimNodes"] = userapi.attr("getSimNodes");
	py::globals()["getSimLinks"] = userapi.attr("getSimLinks");
	py::globals()["deepCopyNode"] = userapi.attr("deepCopyNode");
	py::globals()["deepCopyLink"] = userapi.attr("deepCopyLink");
	py::globals()["deleteEntities"] = userapi.attr("deleteEntities");
	py::globals()["addGroupName"] = userapi.attr("addGroupName");
	py::globals()["addStep"] = userapi.attr("addStep");
	py::globals()["removeStep"] = userapi.attr("removeStep");
	py::globals()["printStep"] = userapi.attr("printStep");
}

void PythonEngineUtil::init_api(py::module_& m, Manager& manager)
{
	// Example: expose addNode
	m.def("addNode", [&manager](float x, float y, float z) {
		auto& node(manager.addEntity<Node>());

		node.addToGroup(Manager::groupNodes_0);
		AssetManager::AddDefaultNode(node, glm::vec3(x, y, z));

		manager.grid->addNode(&node, manager.grid->getGridLevel());

		return EntityIDUtils::toString(node.getId()); // return something to Python
		});

	m.def("addLink", [&manager](py::object fromObj, py::object toObj, float alpha) -> py::object {
		EntityID fromEntityId;
		if (py::isinstance<py::int_>(fromObj)) {
			fromEntityId = fromObj.cast<int>();
		}
		else if (py::isinstance<py::str>(fromObj)) {
			fromEntityId = EntityIDUtils::fromString(fromObj.cast<std::string>());
		}
		else {
			throw std::runtime_error("addLink: fromId must be int or string");
		}

		// Parse toId (accept int or string)
		EntityID toEntityId;
		if (py::isinstance<py::int_>(toObj)) {
			toEntityId = toObj.cast<int>();
		}
		else if (py::isinstance<py::str>(toObj)) {
			toEntityId = EntityIDUtils::fromString(toObj.cast<std::string>());
		}
		else {
			throw std::runtime_error("addLink: toId must be int or string");
		}

		// Extract raw int IDs (adjust if your EntityID variant uses a different type)
		int fromInt = std::get<int>(fromEntityId);
		int toInt = std::get<int>(toEntityId);


		auto& link(manager.addEntity<Link>(fromInt, toInt));

		link.addToGroup(Manager::groupLinks_0);
		link.addComponent<Line_w_Color>();
		link.GetComponent<Line_w_Color>().setSrcColor(TazColor(255, 255, 255, 255.0f * alpha));
		link.GetComponent<Line_w_Color>().setDestColor(TazColor(255, 255, 255, 255.0f * alpha));

		manager.grid->addLink(&link, manager.grid->getGridLevel());

		py::dict result;
		result["id"] = EntityIDUtils::toString(link.getId());
		result["fromId"] = EntityIDUtils::toString(link.getFromNode());
		result["toId"] = EntityIDUtils::toString(link.getToNode());
		return result; // return something to Python
		});

	m.def("getNodes", [&manager]() {
		py::list nodeList;

		std::vector<Entity*> nodes;
		manager.getAllTypeEntities<NodeEntity>(nodes);

		for (auto* node : nodes) {
			py::dict nodeData;
			nodeData["id"] = EntityIDUtils::toString(node->getId());

			// Get position if available
			if (node->hasComponent<TransformComponent>()) {
				auto& transform = node->GetComponent<TransformComponent>();
				auto pos = transform.getPosition();
				nodeData["x"] = pos.x;
				nodeData["y"] = pos.y;
				nodeData["z"] = pos.z;
			}

			nodeList.append(nodeData);
		}

		return nodeList;
		});

	m.def("getSimNodes", []() {
		py::list nodeList;
		auto& simNodes = DataManager::getInstance().mapSimToGraphNodes;

		for (auto& pair : simNodes) {
			py::dict nodeData;
			nodeData["simId"] = pair.first;
			nodeData["id"] = EntityIDUtils::toString(pair.second->getId());

			if (pair.second->hasComponent<TransformComponent>()) {
				auto& tc = pair.second->GetComponent<TransformComponent>();
				auto pos = tc.getPosition();
				nodeData["x"] = pos.x;
				nodeData["y"] = pos.y;
				nodeData["z"] = pos.z;
			}

			nodeList.append(nodeData);
		}

		return nodeList;
		});

	m.def("getSimLinks", []() {
		py::list linkList;
		auto& simLinks = DataManager::getInstance().mapSimToGraphLinks;

		for (auto& pair : simLinks) {
			py::dict linkData;
			linkData["simId"] = pair.first;
			linkData["id"] = EntityIDUtils::toString(pair.second->getId());

			linkData["fromId"] = std::get<int>(pair.second->getFromNode());
			linkData["toId"] = std::get<int>(pair.second->getToNode());

			linkList.append(linkData);
		}

		return linkList;
		});

	m.def("getCurrentStep", [&manager]() -> int {
		if (manager.steps.empty()) {
			return -1;
		}
		// You may need to add a currentStep member to Manager class
		// For now, return the last step index
		return manager.currentStep;
		});

	m.def("deepCopyNode", [&manager](int simId, float alpha) -> py::object {
		auto& simNodes = DataManager::getInstance().mapSimToGraphNodes;
		auto it = simNodes.find(simId);

		if (it == simNodes.end() || !it->second) {
			return py::none();
		}

		NodeEntity* originalNode = it->second;

		// Create new ghost node
		auto& ghostNode = manager.addEntity<Node>();
		ghostNode.addToGroup(Manager::groupNodes_0); // Or a separate ghost group

		// Copy transform
		if (originalNode->hasComponent<TransformComponent>()) {
			auto& origTC = originalNode->GetComponent<TransformComponent>();
			ghostNode.addComponent<TransformComponent>();
			auto& ghostTC = ghostNode.GetComponent<TransformComponent>();
			ghostTC.position = origTC.getPosition();
			ghostTC.size = origTC.size;
		}

		// Copy visual components with transparency
		if (originalNode->hasComponent<RectangleFlashAnimatorComponent>()) {
			auto& origRAC = originalNode->GetComponent<Rectangle_w_Color>();
			ghostNode.addComponent<Rectangle_w_Color>();
			auto& ghostRAC = ghostNode.GetComponent<Rectangle_w_Color>();

			// Copy color but apply alpha
			TazColor color = origRAC.color;
			color.a = origRAC.color.a * alpha;
			ghostRAC.setColor(color);
		}

		// Add to grid
		manager.grid->addNode(&ghostNode, manager.grid->getGridLevel());

		py::dict result;
		result["id"] = EntityIDUtils::toString(ghostNode.getId());
		result["simId"] = simId;

		return result;
		});

	m.def("deepCopyLink", [&manager](int fromSimId, int toSimId, float alpha) -> py::object {
		py::dict result;
		return result;
		});


	m.def("deleteEntities", [&manager](py::list ghostList) -> int {
		int deletedCount = 0;

		for (auto item : ghostList) {
			try {
				py::dict ghost = item.cast<py::dict>();
				if (ghost.contains("id")) {
					std::string id = ghost["id"].cast<std::string>();
					EntityID entityId = EntityIDUtils::fromString(id);

					manager.getEntityFromId(entityId)->destroy();
					deletedCount++;
				}
			}
			catch (...) {
				continue;
			}
		}

		return deletedCount;
		});

	m.def("addGroupName", [&manager](std::string newGroup) -> std::string {
		manager.addGroup(newGroup);
		return newGroup;
		});

	m.def("addStep", [&manager]() -> void {
		DataManager::getInstance().addSimulationStep(manager);

		});

	m.def("addStep", [&manager](sim_dump::UInt32 step, double timestamp, sim_dump::UInt32 copyStep) -> void {
		DataManager::getInstance().addSimulationStep(manager, step, timestamp, copyStep);

		});

	m.def("removeStep", [&manager](sim_dump::UInt32 step) -> void {
		DataManager::getInstance().removeSimulationStep(manager, step);

		});

	m.def("printStep", [&manager](int step) -> void {
		auto text = DataManager::getInstance()
			.simulationStepToString(manager, step);

		py::print(text);

		});
}
