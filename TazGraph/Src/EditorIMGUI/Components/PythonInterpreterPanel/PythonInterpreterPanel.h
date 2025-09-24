#pragma once

#include "../../UIElement.h"

#include <pybind11/pybind11.h>
#include <pybind11/embed.h>

namespace py = pybind11;

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
	std::string _outputText;

	enum console_state {
		Collapsed,
		Expanded
	};

	console_state state = console_state::Collapsed;
	console_state last_state = console_state::Collapsed;
public:

	float default_pythonConsoleHeight = 400.0f;

	ImRect titleBarRect;

	float intervalSec = 1.0f;
	double lastExecTime = 0.0;
	bool autoUpdate = false;

	bool inputActive = false;

	bool init = true;

	ImGuiChildFlags flags = ImGuiChildFlags_ResizeY | ImGuiWindowFlags_NoSavedSettings;

	PythonInterpreterPanel();
	void init_api(py::module_& m, Manager& manager);

	void update(float deltaTime) override;

	void setConfig(const PythonInterpreterConfig& cfg) {


		config = cfg;

	}
	void OnImGuiRender() override;
	void OnImGuiRender2();

	void setFlags();
	void innerTable();
	void runScript();
};