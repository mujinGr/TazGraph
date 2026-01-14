#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
namespace py = pybind11;
class PythonInterpreter
{
public:

	static PythonInterpreter& getInstance() {
		static PythonInterpreter instance;
		return instance;
	}

	PythonInterpreter(const PythonInterpreter&) = delete;
	PythonInterpreter& operator=(const PythonInterpreter&) = delete;

private:

	static std::unique_ptr<py::scoped_interpreter> pythonRuntime;

	PythonInterpreter();


};

