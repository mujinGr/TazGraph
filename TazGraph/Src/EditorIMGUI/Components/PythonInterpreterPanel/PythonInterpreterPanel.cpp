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

	m.def("addLink", [&manager](float x, float y, float z) {
		auto& node(manager.addEntity<Node>());

		node.addGroup(Manager::groupNodes_0);
		AssetManager::AddDefaultNode(node, glm::vec3(x, y, z));

		manager.grid->addNode(&node, manager.grid->getGridLevel());

		return EntityIDUtils::toString(node.getId()); // return something to Python
		});

}

void PythonInterpreterPanel::update(float deltaTime) {
	UIElement::update(deltaTime);

	double now = ImGui::GetTime();

	if (currentScriptType == ScriptType::OnUpdate
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
		ImGui::SetNextWindowPos(ImVec2(config.viewportPos->x, config.viewportPos->y + config.viewportSize->y - titleBarRect.GetHeight()));
	}

	if (state == console_state::Expanded) {

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.f, 4.f));
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.9f);

		ImGui::Begin("Python Interpreter", nullptr, flags);

		innerTable();

		setFlags();

		ImGui::End();

		ImGui::PopStyleVar(2);

	}
	else if (state == console_state::Collapsed) {
		ImGui::Begin("Python Interpreter", nullptr, flags);

		if (init) {
			ImGui::SetWindowCollapsed(ImGui::GetCurrentWindow());

			init = false;
		}
		innerTable();

		setFlags();

		ImGui::End();
	}
}

void PythonInterpreterPanel::OnImGuiRender2() {

	ImGui::BeginChild("Python Interpreter");
	ImGuiChildFlags flags = ImGuiChildFlags_ResizeY;

	ImGui::BeginChild("Python Input", ImVec2(0.0f, 300.0f), flags);
	ImGui::Text("Python Script");

	float originalScale = ImGui::GetFont()->Scale;
	ImGui::GetFont()->Scale = 1.5f;
	ImGui::PushFont(ImGui::GetFont());

	ImGui::InputTextMultiline("##pythonInput",
		_pythonBuffer, IM_ARRAYSIZE(_pythonBuffer),
		ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 4 * 1.5f));

	inputActive = ImGui::IsItemActive() || ImGui::IsItemFocused();

	ImGui::PopFont();
	ImGui::GetFont()->Scale = originalScale;

	if (currentScriptType == ScriptType::OneOff) {
		if (ImGui::Button("Run"))
		{
			runScript();
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear")) {
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
			ImGui::TextDisabled("(Runs every frame)");
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear Output")) {
			_updateOutputText.clear();
		}
	}
	ImGui::Separator();
	ImGui::Text("Auto-run:");
	ImGui::Checkbox("Use Seconds", &useInterval);
	ImGui::InputFloat("Interval (s)", &intervalSec, 0.1f, 1.0f, "%.2f");

	ImGui::EndChild();


	ImGui::BeginChild("Python Output");
	ImGui::Text("Output:");
	ImGui::BeginChild("OutputChild", ImVec2(0.0f, 100.0f), true);
	ImGui::TextWrapped("%s", _outputText.c_str());
	ImGui::EndChild();
	ImGui::EndChild();


	ImGui::EndChild();
}

void PythonInterpreterPanel::setFlags() {
	last_state = state;

	if (ImGui::IsWindowCollapsed()) {
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
		}

		float originalScale = ImGui::GetFont()->Scale;
		ImGui::GetFont()->Scale = 1.5f;
		ImGui::PushFont(ImGui::GetFont());

		if (currentScriptType == ScriptType::OneOff) {
			ImGui::InputTextMultiline("##pythonInput",
				_pythonBuffer, IM_ARRAYSIZE(_pythonBuffer),
				ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 10 * 1.5f));
		}
		else {
			ImGui::InputTextMultiline("##updateInput",
				_updateBuffer, IM_ARRAYSIZE(_updateBuffer),
				ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 10 * 1.5f));
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
		ImGui::BeginChild("OutputChild", ImVec2(0, 0), true);
		if (currentScriptType == ScriptType::OneOff) {
			ImGui::TextWrapped("%s", _outputText.c_str());
		}
		else {
			ImGui::TextWrapped("%s", _updateOutputText.c_str());
		}
		ImGui::EndChild();

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
		py::globals()["addNode"] = userapi.attr("addNode");
		py::exec(_pythonBuffer);
		py::object output = py::eval("sys.stdout.getvalue()");
		_outputText = output.cast<std::string>();

		config.scene->manager->aboutTo_updateActiveEntities();
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
		py::globals()["deltaTime"] = deltaTime;

		py::exec(_updateBuffer);

		py::object output = py::eval("sys.stdout.getvalue()");
		_updateOutputText = output.cast<std::string>();

		config.scene->manager->aboutTo_updateActiveEntities();
	}
	catch (const std::exception& e) {
		_updateOutputText = std::string("Python error: ") + e.what();
	}
}