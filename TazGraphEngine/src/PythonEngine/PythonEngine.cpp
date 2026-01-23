#include "PythonEngine.h"

namespace py = pybind11;

// Definition with initialization to nullptr
std::unique_ptr<py::scoped_interpreter> PythonInterpreter::pythonRuntime = nullptr;

PythonInterpreter::PythonInterpreter()
{
	if (!pythonRuntime) {
		pythonRuntime = std::make_unique<py::scoped_interpreter>();
	}

}
