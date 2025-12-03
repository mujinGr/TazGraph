#include "PythonInterpreterPanel.h"


static std::unique_ptr<py::scoped_interpreter> pythonRuntime;

PythonInterpreterPanel::PythonInterpreterPanel()
{
	if (!pythonRuntime)
		pythonRuntime = std::make_unique<py::scoped_interpreter>();
}

void PythonInterpreterPanel::init_api(py::module_& m, Manager& manager)
{
	// Example: expose addNode
	m.def("addNode", [&manager](float x, float y, float z) {
		auto& node(manager.addEntity<Node>());

		node.addGroup(Manager::groupNodes_0);
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

		link.addGroup(Manager::groupLinks_0);
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
		ghostNode.addGroup(Manager::groupNodes_0); // Or a separate ghost group

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


}

void PythonInterpreterPanel::update(float deltaTime) {
	UIElement::update(deltaTime);

	double now = ImGui::GetTime();

	if (!updatePaused &&
		currentScriptType == ScriptType::OnUpdate
		&& now - lastExecTime >= intervalSec) {
		runUpdateScript(deltaTime);
		lastExecTime = now;
	}
	//else {
	//	// Execute update script every frame when auto-update is off
	//	if (strlen(_updateBuffer) > 0) {
	//		runUpdateScript(deltaTime);
	//	}
	//}
}

void PythonInterpreterPanel::OnImGuiRender()
{
	if (ImGui::FindWindowByName("Python Interpreter") && state == console_state::Expanded)
	{
		ImGui::SetNextWindowSize(ImVec2(config.viewportSize->x, ImGui::FindWindowByName("Python Interpreter")->Size.y));
		ImGui::SetNextWindowPos(ImVec2(config.viewportPos->x, config.viewportPos->y + config.viewportSize->y - ImGui::FindWindowByName("Python Interpreter")->Size.y));
	}
	if (last_state == console_state::Collapsed && state == console_state::Expanded)
	{
		ImGui::SetNextWindowSize(ImVec2(config.viewportSize->x, default_pythonConsoleHeight));
		ImGui::SetNextWindowPos(ImVec2(config.viewportPos->x, config.viewportPos->y + config.viewportSize->y - default_pythonConsoleHeight));
	}
	if (ImGui::FindWindowByName("Python Interpreter") && state == console_state::Collapsed)
	{
		ImGui::SetNextWindowSize(ImVec2(config.viewportSize->x, titleBarRect.GetHeight()));
		if (titleBarRect.GetHeight() == 0) {
			ImGui::SetNextWindowPos(ImVec2(config.viewportPos->x, config.viewportPos->y + config.viewportSize->y - 20.0f));
		}
		else {
			ImGui::SetNextWindowPos(ImVec2(config.viewportPos->x, config.viewportPos->y + config.viewportSize->y - titleBarRect.GetHeight()));

		}
	}

	// Set collapsed state on first frame
	if (init) {
		ImGui::SetNextWindowCollapsed(true, ImGuiCond_Once);
	}

	if (state == console_state::Expanded) {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.f, 4.f));
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.9f);

		bool widgetActive = ImGui::Begin("Python Interpreter", nullptr, flags);

		// Clear init flag after first Begin
		if (init) {
			init = false;
		}

		if (widgetActive)
		{
			innerTable();
		}
		setFlags();

		ImGui::End();
		ImGui::PopStyleVar(2);
	}
	else if (state == console_state::Collapsed) {
		bool widgetActive = ImGui::Begin("Python Interpreter", nullptr, flags);

		// Clear init flag after first Begin
		if (init) {
			init = false;
		}

		if (widgetActive) {
			innerTable();
		}
		setFlags();

		ImGui::End();
	}
}

void PythonInterpreterPanel::OnImGuiRender2() {

	ImGui::Text("Script Type:");
	ImGui::SameLine();
	if (ImGui::RadioButton("One-Off", currentScriptType == ScriptType::OneOff)) {
		currentScriptType = ScriptType::OneOff;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("On Update", currentScriptType == ScriptType::OnUpdate)) {
		currentScriptType = ScriptType::OnUpdate;
	}

	ImGui::Separator();
	bool childActive = ImGui::BeginChild("Python Interpreter");
	if (childActive) {
		ImGuiChildFlags flags = ImGuiChildFlags_ResizeY;
		bool nestedChildActive = ImGui::BeginChild("Python Input", ImVec2(0.0f, 300.0f), flags);
		if (nestedChildActive) {
			ImGui::Text("Python Script");
			if (currentScriptType == ScriptType::OneOff) {
				ImGui::Text("One-Off Script (Execute on demand)");
			}
			else {
				ImGui::Text("Update Script (Execute every frame)");
				ImGui::Checkbox("Pause Update", &updatePaused);
			}
			float originalScale = ImGui::GetFont()->Scale;
			ImGui::GetFont()->Scale = 1.5f;
			ImGui::PushFont(ImGui::GetFont());

			if (currentScriptType == ScriptType::OneOff) {
				ImGui::InputTextMultiline("##pythonInput",
					_pythonBuffer, IM_ARRAYSIZE(_pythonBuffer),
					ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 4 * 1.5f));
			}
			else {
				ImGui::InputTextMultiline("##updateInput",
					_updateBuffer, IM_ARRAYSIZE(_updateBuffer),
					ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 4 * 1.5f));
			}

			inputActive = ImGui::IsItemActive() || ImGui::IsItemFocused();

			ImGui::PopFont();
			ImGui::GetFont()->Scale = originalScale;

		}
		ImGui::EndChild(); //? Needs to be outside

		if (currentScriptType == ScriptType::OneOff) {
			if (ImGui::Button("Run")) {
				runScript();
			}
			ImGui::SameLine();
			if (ImGui::Button("Clear Output")) {
				_outputText.clear();
			}
		}
		else {
			ImGui::Text("Auto-run settings:");
			ImGui::Checkbox("Use Interval", &useInterval);
			if (useInterval) {
				ImGui::InputFloat("Interval (s)", &intervalSec, 0.1f, 1.0f, "%.2f");
			}
			else {
				intervalSec = 0.0f;
				ImGui::TextDisabled("(Runs every frame)");
			}

			if (ImGui::Button("Clear Output")) {
				_updateOutputText.clear();
			}
		}
		nestedChildActive = ImGui::BeginChild("Python Output");
		if (nestedChildActive) {
			ImGui::Text("Output:");
			bool nestedNestedChildActive = ImGui::BeginChild("OutputChild", ImVec2(0.0f, 100.0f), true);
			if (nestedNestedChildActive)
			{
				if (currentScriptType == ScriptType::OneOff) {
					ImGui::TextWrapped("%s", _outputText.c_str());
				}
				else {
					ImGui::TextWrapped("%s", _updateOutputText.c_str());
				}
			}
			ImGui::EndChild(); //? Needs to be outside
		}
		ImGui::EndChild(); //? Needs to be outside
	}
	ImGui::EndChild(); //? Needs to be outside
}

void PythonInterpreterPanel::setFlags() {
	last_state = state;
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	bool collapsed = window->Collapsed;
	if (collapsed) {
		flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
		state = console_state::Collapsed;
	}
	else { // ---Expanded

		flags |= ImGuiWindowFlags_NoResize;

		state = console_state::Expanded;

		titleBarRect = ImGui::GetCurrentWindow()->TitleBarRect();

		float collisionPadding = 5.0f;

		ImVec2 min = ImVec2(titleBarRect.Min.x - collisionPadding, titleBarRect.Min.y - collisionPadding);
		ImVec2 max = ImVec2(titleBarRect.Max.x + collisionPadding, titleBarRect.Max.y + collisionPadding);
		ImVec2 mouse = ImGui::GetMousePos();

		if (mouse.x + collisionPadding > min.x && mouse.x < max.x && mouse.y > min.y && mouse.y < max.y)
		{
			flags = ImGuiChildFlags_ResizeY | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
		}
	}

}

void PythonInterpreterPanel::innerTable() {
	ImGui::Text("Script Type:");
	ImGui::SameLine();
	if (ImGui::RadioButton("One-Off", currentScriptType == ScriptType::OneOff)) {
		currentScriptType = ScriptType::OneOff;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("On Update", currentScriptType == ScriptType::OnUpdate)) {
		currentScriptType = ScriptType::OnUpdate;
	}

	ImGui::Separator();

	if (ImGui::BeginTable("PythonIO", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV))
	{
		// ===== LEFT COLUMN: Input =====
		ImGui::TableNextColumn();
		if (currentScriptType == ScriptType::OneOff) {
			ImGui::Text("One-Off Script (Execute on demand)");
		}
		else {
			ImGui::Text("Update Script (Execute every frame)");
			ImGui::Checkbox("Pause Update", &updatePaused);
		}

		// Calculate height for text input
		float buttonHeight = ImGui::GetFrameHeightWithSpacing() * 2; // Space for buttons below
		float settingsHeight = 0.0f;

		if (currentScriptType == ScriptType::OnUpdate) {
			// Account for auto-run settings
			settingsHeight = ImGui::GetFrameHeightWithSpacing() * 3; // Text + Checkbox + InputFloat
		}

		float availableHeight = ImGui::GetContentRegionAvail().y - buttonHeight - settingsHeight;

		float originalScale = ImGui::GetFont()->Scale;
		ImGui::GetFont()->Scale = 1.5f;
		ImGui::PushFont(ImGui::GetFont());

		if (currentScriptType == ScriptType::OneOff) {
			ImGui::InputTextMultiline("##pythonInput",
				_pythonBuffer, IM_ARRAYSIZE(_pythonBuffer),
				ImVec2(-FLT_MIN, availableHeight));
		}
		else {
			ImGui::InputTextMultiline("##updateInput",
				_updateBuffer, IM_ARRAYSIZE(_updateBuffer),
				ImVec2(-FLT_MIN, availableHeight));
		}

		inputActive = ImGui::IsItemActive() || ImGui::IsItemFocused();

		ImGui::PopFont();
		ImGui::GetFont()->Scale = originalScale;

		if (currentScriptType == ScriptType::OneOff) {
			if (ImGui::Button("Run")) {
				runScript();
			}
			ImGui::SameLine();
			if (ImGui::Button("Clear Output")) {
				_outputText.clear();
			}
		}
		else {
			ImGui::Text("Auto-run settings:");
			ImGui::Checkbox("Use Interval", &useInterval);
			if (useInterval) {
				ImGui::InputFloat("Interval (s)", &intervalSec, 0.1f, 1.0f, "%.2f");
			}
			else {
				intervalSec = 0.0f;
				ImGui::TextDisabled("(Runs every frame)");
			}

			if (ImGui::Button("Clear Output")) {
				_updateOutputText.clear();
			}
		}

		// ===== RIGHT COLUMN: Output =====
		ImGui::TableNextColumn();
		ImGui::Text("Output:");
		bool childActive = ImGui::BeginChild("OutputChild", ImVec2(0, 0), true);
		if (childActive) {
			if (currentScriptType == ScriptType::OneOff) {
				ImGui::TextWrapped("%s", _outputText.c_str());
			}
			else {
				ImGui::TextWrapped("%s", _updateOutputText.c_str());
			}
		}
		ImGui::EndChild(); //? Needs to be outside

		ImGui::EndTable();
	}
}


void PythonInterpreterPanel::runScript() {
	safe_putenv("PYTHONHOME=C:\\Users\\lefte\\AppData\\Local\\Programs\\Python\\Python313");
	try {
		py::exec(R"(
				import sys
				from io import StringIO
				sys.stdout = StringIO()
			)");

		py::module_ userapi = py::module_::create_extension_module("tazpyapi", nullptr, new PyModuleDef{});
		init_api(userapi, *config.scene->manager);
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
		py::exec(_pythonBuffer);
		py::object output = py::eval("sys.stdout.getvalue()");
		_outputText = output.cast<std::string>();

	}
	catch (const std::exception& e) {
		_outputText = std::string("Python error: ") + e.what();
	}
}

void PythonInterpreterPanel::runUpdateScript(float deltaTime) {
	safe_putenv("PYTHONHOME=C:\\Users\\lefte\\AppData\\Local\\Programs\\Python\\Python313");
	try {
		py::exec(R"(
				import sys
				from io import StringIO
				sys.stdout = StringIO()
			)");

		py::module_ userapi = py::module_::create_extension_module("tazpyapi", nullptr, new PyModuleDef{});
		init_api(userapi, *config.scene->manager);
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
		py::globals()["deltaTime"] = deltaTime;

		py::exec(_updateBuffer);

		py::object output = py::eval("sys.stdout.getvalue()");
		_updateOutputText = output.cast<std::string>();

	}
	catch (const std::exception& e) {
		_updateOutputText = std::string("Python error: ") + e.what();
	}
}