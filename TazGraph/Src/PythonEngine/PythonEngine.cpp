#include "./PythonEngine.h"

namespace py = pybind11;

static std::unique_ptr<py::scoped_interpreter> pythonRuntime;

PythonInterpreter::PythonInterpreter()
{
	if (!pythonRuntime)
		pythonRuntime = std::make_unique<py::scoped_interpreter>();
}