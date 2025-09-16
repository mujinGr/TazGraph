#pragma once

#include "../../UIElement.h"

#include <pybind11/pybind11.h>
#include <pybind11/embed.h>

namespace py = pybind11;

struct PythonInterpreterConfig {
	IScene* scene;
};

class PythonInterpreterPanel : public UIElement
{
private:
	PythonInterpreterConfig  config;
	char _pythonBuffer[1024] = "";
	std::string _outputText;
public:
	ImVec2 buttonSize = ImVec2(100.0f, 30.0f);
	bool isCollapsed = false;
	bool showPythonInterpreter = false;
	PythonInterpreterPanel();
	void init_api(py::module_& m, Manager& manager);

	void setConfig(const PythonInterpreterConfig& cfg) {


		config = cfg;

	}
	void OnImGuiRender() override;
};