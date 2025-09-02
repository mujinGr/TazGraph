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

public:
	bool showPythonInterpreter = false;
	void setConfig(const PythonInterpreterConfig& cfg) { config = cfg; }
	void OnImGuiRender() override;
};