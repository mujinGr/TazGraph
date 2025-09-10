#pragma once

#include "../../UIElement.h"

#include <pybind11/pybind11.h>
#include <pybind11/embed.h>

namespace py = pybind11;

struct PythonInterpreterConfig {
};

class PythonInterpreterPanel : public UIElement
{
private:
	PythonInterpreterConfig  config;
	char _pythonBuffer[1024] = "";
	std::string _outputText;
public:
	bool showPythonInterpreter = false;

	void init_api(py::module_& m);

	void setConfig(const PythonInterpreterConfig& cfg) { 
		

		config = cfg; 
	
	}
	void OnImGuiRender() override;
};