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

	float pythonConsoleHeight = 400.0f;

	ImRect titleBarRect;

	ImGuiChildFlags flags = ImGuiChildFlags_ResizeY;

	PythonInterpreterPanel();
	void init_api(py::module_& m, Manager& manager);

	void setConfig(const PythonInterpreterConfig& cfg) {


		config = cfg;

	}
	void OnImGuiRender() override;
	void setFlags();
	void innerTable();
};