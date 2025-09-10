#include "PythonInterpreterPanel.h"

void PythonInterpreterPanel::init_api(py::module_& m)
{
	// Example: expose addNode
	m.def("addNode", [](Manager& manager, float x, float y, float z) {
		//auto& node(manager.addEntity<Node>());

		//node.addGroup(Manager::groupNodes_0);

		//node.addComponent<TransformComponent>(glm::vec3(x, y, z), Layer::action, glm::vec3(10.0f), 1.0f);
		/*node.addComponent<Rectangle_w_Color>();
		node.GetComponent<Rectangle_w_Color>().setColor(Color(0, 200, 224, 255));*/

		//return &node;
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
				py::scoped_interpreter guard{};
				py::exec(R"(
                    import sys
                    from io import StringIO
                    sys.stdout = StringIO()
                )");
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
