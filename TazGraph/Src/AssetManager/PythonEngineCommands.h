#pragma once

#include <src/GECS/Core/GECSManager.h>

#include "AssetManager.h"

class PythonEngineCommands
{
public:
	PythonEngineCommands();


	void init(Manager& manager);
	void init_api(py::module_& m, Manager& manager);
};

