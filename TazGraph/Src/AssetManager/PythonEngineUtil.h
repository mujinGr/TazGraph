#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/embed.h>

#include <src/GECS/Core/GECSManager.h>

#include "AssetManager.h"

namespace py = pybind11;

class PythonEngineUtil
{
public:
	PythonEngineUtil();


	void init(Manager& manager);
	void init_api(py::module_& m, Manager& manager);
};

