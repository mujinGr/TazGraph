#include "EntityComponentsControl.h"

void EntityComponentsControlPanel::OnImGuiRender()
{

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	if (!config.displayedEntity) return;

	std::string windowTitle = "Entity";
	Node* node = dynamic_cast<Node*>(config.displayedEntity);
	Link* link = dynamic_cast<Link*>(config.displayedEntity);
	Empty* empty = dynamic_cast<Empty*>(config.displayedEntity);


	if (node) {
		windowTitle = "Node Display";
	}
	else if (link) {
		windowTitle = "Link Display";
	}
	else if (empty) {
		windowTitle = "Empty Display";
	}

	EntityID currentId = config.displayedEntity->getId();

	Manager* man = config.scene->manager;

	if (ImGui::BeginChild(windowTitle.c_str())) {

		config.displayedEntity->imgui_print();

		ImGui::Separator();

		auto sortComponentsByID = [](const std::vector<std::string>& componentNames) {
			std::vector<std::pair<ComponentID, std::string>> sorted;
			for (const auto& name : componentNames) {
				auto it = componentNameToID.find(name);
				if (it != componentNameToID.end()) {
					sorted.emplace_back(it->second, name);
				}
			}
			std::sort(sorted.begin(), sorted.end(),
				[](const auto& a, const auto& b) {
					return a.first < b.first;
				});

			std::vector<std::string> result;
			for (const auto& [id, name] : sorted) {
				result.push_back(name);
			}
			return result;
			};


		if (node)
		{
			for (auto& c : sortComponentsByID(config.scene->manager->componentNames["Component"])) {
				ComponentCheckbox(c);
			}
			ImGui::Separator();
			for (auto& c : sortComponentsByID(config.scene->manager->componentNames["NodeComponent"])) {
				ComponentCheckbox(c);
			}
		}

		else if (link) {
			for (auto& c : sortComponentsByID(config.scene->manager->componentNames["LinkComponent"])) {
				ComponentCheckbox(c);
			}
		}

		else if (empty) {
			for (auto& c : sortComponentsByID(config.scene->manager->componentNames["Component"])) {
				ComponentCheckbox(c);
			}
		}
	}

	ImGui::EndChild();

}


void EntityComponentsControlPanel::ComponentCheckbox(std::string c) {
	bool hasComponent = config.displayedEntity->hasComponentByName(c);

	auto it = componentNameToID.find(c);
	if (it != componentNameToID.end()) {
		ComponentID cid = it->second;

		if (cid == GetComponentTypeID<EmptyPythonCodeComponent>() ||
			cid == GetComponentTypeID<LinkPythonCodeComponent>()) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 1.0f, 1.0f)); // cyan
		}

		ImGui::Text("(ID: %u)", cid);
		ImGui::SameLine();

	}

	if (ImGui::Checkbox(c.c_str(), &hasComponent)) {
		if (hasComponent) {
			AddComponentByName(c, config.displayedEntity);
		}
		else {
			RemoveComponentByName(c, config.displayedEntity);
		}
	}

	if (c == "EmptyPythonCodeComponent" ||
		c == "LinkPythonCodeComponent") {
		ImGui::PopStyleColor(); // cyan
	}

	if (hasComponent) {
		if (ImGui::TreeNode((c + " Properties").c_str())) {
			getComponentByName(c, config.displayedEntity)->showGUI();
			ImGui::TreePop();
		}
	}
}
