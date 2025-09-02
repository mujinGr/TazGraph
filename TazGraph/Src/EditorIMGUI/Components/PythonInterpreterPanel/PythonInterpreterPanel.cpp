#include "PythonInterpreterPanel.h"

void PythonInterpreterPanel::OnImGuiRender()
{
	ImGui::Begin("Python Interpreter", &showPythonInterpreter);

	ImGui::Text("Write Python code here:");
	ImGui::InputTextMultiline("##pythonInput", _pythonBuffer, IM_ARRAYSIZE(_pythonBuffer), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16));

	if (ImGui::Button("Run"))
	{
		// Here you would pass pythonBuffer to your interpreter
		// For now just print to console
		printf("Python input: %s\n", _pythonBuffer);
		py::scoped_interpreter guard{};
		py::exec("print('Hello from Python via pybind11')");
	}

	ImGui::SameLine();
	if (ImGui::Button("Clear"))
	{
		_pythonBuffer[0] = '\0';
	}

	ImGui::End();
}
