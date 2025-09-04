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
		_putenv("PYTHONHOME=C:\\Users\\lefte\\AppData\\Local\\Programs\\Python\\Python313");
		try {
			py::scoped_interpreter guard{}; // initializes interpreter
			py::exec(R"(
			import sys
			from io import StringIO
			sys.stdout = StringIO()
			)");
			// Run whatever the user typed
			py::exec(_pythonBuffer);
			py::object output = py::eval("sys.stdout.getvalue()");
			std::string result = output.cast<std::string>();

			_outputText = result;
		}
		catch (const std::exception& e) {
			// Print errors to console (or redirect to ImGui window later)
			printf("Python error: %s\n", e.what());
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("Clear"))
	{
		_pythonBuffer[0] = '\0';
		_outputText.clear();
	}

	// Display output
	ImGui::Separator();
	ImGui::Text("Output:");
	ImGui::BeginChild("Output", ImVec2(0, ImGui::GetTextLineHeight() * 10), true);
	ImGui::TextWrapped("%s", _outputText.c_str());
	ImGui::EndChild();

	ImGui::End();
}
