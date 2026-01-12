#include "PythonInterpreterPanel.h"


//!!!!!! Python to run needs to be on the context of scene, not render thread

namespace py = pybind11;

PythonInterpreterPanel::PythonInterpreterPanel()
{
}

void PythonInterpreterPanel::update(float deltaTime) {
	UIElement::update(deltaTime);

	if (readyToExecute) {
		runScript();
		readyToExecute = false;
	}

	if (readyToClear) {
		clearOutput();
		readyToClear = false;
	}

	double now = ImGui::GetTime();

	if (!updatePaused &&
		currentScriptType == ScriptType::OnUpdate
		&& now - lastExecTime >= intervalSec) {
		runUpdateScript(deltaTime);
		lastExecTime = now;
	}
	py::gil_scoped_acquire gil;

	py::module_ sys = py::module_::import("sys"); //!? keep this
	_stdout_buffer = sys.attr("stdout");

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
	if (firstLoop) {
		ImGui::SetNextWindowCollapsed(true, ImGuiCond_Once);
	}

	if (state == console_state::Expanded) {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.f, 4.f));
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.9f);

		bool widgetActive = ImGui::Begin("Python Interpreter", nullptr, flags);

		// Clear init flag after first Begin
		if (firstLoop) {
			firstLoop = false;
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
		if (firstLoop) {
			firstLoop = false;
		}

		if (widgetActive) {
			innerTable();
		}
		setFlags();

		ImGui::End();
	}
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
				readyToExecute = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("Clear Output")) {
				readyToClear = true;
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
				readyToClear = true;
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

void PythonInterpreterPanel::clearOutput() {
	_updateOutputText.clear();
	_outputText.clear();

	try {
		_stdout_buffer.attr("truncate")(0);
		_stdout_buffer.attr("seek")(0);
	}
	catch (const std::exception& e) {
		std::cerr << "Failed to clear StringIO: " << e.what() << std::endl;
	}
}

void PythonInterpreterPanel::runScript() {
	//! safe_putenv("PYTHONHOME=C:\\Users\\lefte\\AppData\\Local\\Programs\\Python\\Python313");
	try {

		py::exec(_pythonBuffer);
		py::object output = _stdout_buffer.attr("getvalue")();
		_outputText = output.cast<std::string>();

	}
	catch (const std::exception& e) {
		_outputText = std::string("Python error: ") + e.what();
	}
}

void PythonInterpreterPanel::runUpdateScript(float deltaTime) {
	try {

		py::globals()["deltaTime"] = deltaTime;

		py::exec(_updateBuffer);

		py::object output = _stdout_buffer.attr("getvalue")();
		_updateOutputText = output.cast<std::string>();

	}
	catch (const std::exception& e) {
		_updateOutputText = std::string("Python error: ") + e.what();
	}
}