#pragma once

#include "../../UIElement.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"

struct PythonInterpreterConfig {
	IScene* scene;
	ImVec2* viewportPos;
	ImVec2* viewportSize;
};

class PythonInterpreterPanel : public UIElement
{
private:
	PythonInterpreterConfig  config;
	char _pythonBuffer[1024] = "";
	char _updateBuffer[4096] = "";

	py::object _stdout_buffer;

	std::string _outputText;
	std::string _updateOutputText;

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

	float default_pythonConsoleHeight = 400.0f;

	ImRect titleBarRect;

	float intervalSec = 1.0f;
	double lastExecTime = 0.0;
	bool useInterval = false;
	bool updatePaused = true;

	bool inputActive = false;

	bool firstLoop = true;

	bool readyToClear = false;
	bool readyToExecute = false;

	ImGuiChildFlags flags = ImGuiWindowFlags_NoSavedSettings;

	PythonInterpreterPanel();
	void update(float deltaTime) override;

	void setConfig(const PythonInterpreterConfig& cfg) {


		config = cfg;

	}
	void OnImGuiRender() override;

	void setFlags();
	void innerTable();
	void clearOutput();
	void runScript();
	void runUpdateScript(float deltaTime);
};

#pragma GCC diagnostic pop