#pragma once

#include "../Components.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"

class PythonCodeComponent
{
private:
	char _pythonBuffer[1024] = "";
	char _updateBuffer[4096] = "";

	std::string _outputText;
	std::string _updateOutputText;

	py::object _stdout_buffer;
	py::module_ sys;

	enum console_state {
		Collapsed,
		Expanded
	};

	enum class ScriptType {
		OneOff,
		OnUpdate
	};

	ScriptType currentScriptType = ScriptType::OneOff;

	console_state state = console_state::Collapsed;
	console_state last_state = console_state::Collapsed;

public:

	float intervalSec = 1.0f;
	double lastExecTime = 0.0;
	bool useInterval = false;
	bool updatePaused = true;

	bool inputActive = false;

	bool readyToClear = false;
	bool readyToExecute = false;

	PythonCodeComponent()
	{
	}


	void init()
	{
	}
	void update(float deltaTime)
	{
		py::gil_scoped_acquire gil;
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

		sys = py::module_::import("sys"); //!? keep this
		_stdout_buffer = sys.attr("stdout");
		_stdout_buffer = sys.attr("stdout");
	}

	std::string GetComponentName() {
		return "PythonCodeComponent";
	}

	void showGUI(std::vector<BaseComponent*> otherComponents = {}) {
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
	};

	void clearOutput() {
		try {
			_stdout_buffer.attr("truncate")(0);
			_stdout_buffer.attr("seek")(0);
		}
		catch (const std::exception& e) {
			std::cerr << "Failed to clear StringIO: " << e.what() << std::endl;
		}
	}

	void runScript() {
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

	void runUpdateScript(float deltaTime) {
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
};

#pragma GCC diagnostic pop