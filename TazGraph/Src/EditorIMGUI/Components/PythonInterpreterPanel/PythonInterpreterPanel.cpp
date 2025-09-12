#include "PythonInterpreterPanel.h"


static std::unique_ptr<py::scoped_interpreter> pythonRuntime;

PythonInterpreterPanel::PythonInterpreterPanel()
{
	if (!pythonRuntime)
		pythonRuntime = std::make_unique<py::scoped_interpreter>();
}

void PythonInterpreterPanel::init_api(py::module_& m, Manager& manager)
{
	// Example: expose addNode
	m.def("addNode", [&manager](float x, float y, float z) {
		auto& node(manager.addEntity<Node>());

		node.addGroup(Manager::groupNodes_0);
		AssetManager::AddDefaultNode(node, glm::vec3(x,y,z));

		manager.grid->addNode(&node, manager.grid->getGridLevel());

		return node.getId(); // return something to Python
		});

}

void PythonInterpreterPanel::OnImGuiRender()
{
	ImGui::BeginChild("Python Interpreter");
	// Create a 2-column table: input (left), output (right)
	if (ImGui::BeginTable("PythonIO", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV))
	{
		// ===== LEFT COLUMN: Input =====
		ImGui::TableNextColumn();
		ImGui::Text("Python Script");

		float originalScale = ImGui::GetFont()->Scale;
		ImGui::GetFont()->Scale = 1.5f;
		ImGui::PushFont(ImGui::GetFont());

		ImGui::InputTextMultiline("##pythonInput",
			_pythonBuffer, IM_ARRAYSIZE(_pythonBuffer),
			ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 10 * 1.5f));

		ImGui::PopFont();
		ImGui::GetFont()->Scale = originalScale;

		if (ImGui::Button("Run"))
		{
			safe_putenv("PYTHONHOME=C:\\Users\\lefte\\AppData\\Local\\Programs\\Python\\Python313");
			try {
				py::exec(R"(
                    import sys
                    from io import StringIO
                    sys.stdout = StringIO()
                )");

				py::module_ userapi = py::module_::create_extension_module("tazpyapi", nullptr, new PyModuleDef{});
				init_api(userapi, *config.scene->manager);
				py::module_::import("sys").attr("modules")["tazpyapi"] = userapi;

				py::globals()["addNode"] = userapi.attr("addNode");
				py::exec(_pythonBuffer);
				py::object output = py::eval("sys.stdout.getvalue()");
				_outputText = output.cast<std::string>();
			}
			catch (const std::exception& e) {
				_outputText = std::string("Python error: ") + e.what();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear")) {
			_outputText.clear();
		}

		// ===== RIGHT COLUMN: Output =====
		ImGui::TableNextColumn();
		ImGui::Text("Output:");
		ImGui::BeginChild("OutputChild", ImVec2(0, 0), true);
		ImGui::TextWrapped("%s", _outputText.c_str());
		ImGui::EndChild();

		ImGui::EndTable();
	}

	ImGui::EndChild();
}
