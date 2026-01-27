#include "EntityComponentsControl.h"

void EntityComponentsControlPanel::OnImGuiRender()
{
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	if (!displayedEntities[0]) return;

	std::string windowTitle = "Entity";

	NodeEntity* node = dynamic_cast<NodeEntity*>(displayedEntities[0]);
	LinkEntity* link = dynamic_cast<LinkEntity*>(displayedEntities[0]);
	EmptyEntity* empty = dynamic_cast<EmptyEntity*>(displayedEntities[0]);


	if (node) {
		windowTitle = "Node Display";
	}
	else if (link) {
		windowTitle = "Link Display";
	}
	else if (empty) {
		windowTitle = "Empty Display";
	}

	bool allSameType = config.scene->manager->entities_AllSameType(displayedEntities);

	if (!allSameType) {
		ImGui::Text("Multiple selection with different entity types");
		return;
	}

	EntityID currentId = displayedEntities[0]->getId();

	Manager* man = config.scene->manager;

	if (ImGui::BeginChild(windowTitle.c_str())) {

		displayedEntities[0]->imgui_print();

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
	int entitiesWithComponent = 0;
	for (Entity* entity : displayedEntities) {
		if (entity->hasComponentByName(c)) {
			entitiesWithComponent++;
		}
	}
	bool hasComponent = entitiesWithComponent > 0;
	bool mixedState = entitiesWithComponent > 0 && entitiesWithComponent < displayedEntities.size();

	auto it = componentNameToID.find(c);
	ComponentID cid = (it != componentNameToID.end()) ? it->second : 0;

	bool isPythonComponent = (cid == GetComponentTypeID<EmptyPythonCodeComponent>() ||
		cid == GetComponentTypeID<LinkPythonCodeComponent>());

	if (isPythonComponent) {
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 1.0f, 1.0f)); // cyan
	}

	if (it != componentNameToID.end()) {
		ImGui::Text("(ID: %lu)", cid);
		ImGui::SameLine();
	}

	if (mixedState) {
		ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(1.0f, 0.6f, 0.0f, 1.0f)); // Orange
		ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, true);
	}

	bool componentEnabled = hasComponent;
	if (ImGui::Checkbox(c.c_str(), &componentEnabled)) {
		// Add or remove component from all entities
		for (Entity* entity : displayedEntities) {
			if (componentEnabled && !entity->hasComponentByName(c)) {
				AddComponentByName(c, entity);
			}
			else if (!componentEnabled && entity->hasComponentByName(c)) {
				RemoveComponentByName(c, entity);
			}
		}
	}

	if (mixedState) {
		ImGui::PopItemFlag();
		ImGui::PopStyleColor(); // Pop orange checkmark color
	}

	if (isPythonComponent) {
		ImGui::PopStyleColor();
	}


	if (hasComponent) {
		if (ImGui::TreeNode((c + " Properties").c_str())) {
			if (displayedEntities.size() == 1) {
				getComponentByName(c, displayedEntities[0])->showGUI();
			}
			else {
				// Find first entity with the component
				Entity* referenceEntity = nullptr;
				for (Entity* entity : displayedEntities) {
					if (entity->hasComponentByName(c)) {
						referenceEntity = entity;
						break;
					}
				}

				// Show GUI of first entity
				if (referenceEntity) {
					// Modify all entities in the group
					getComponentByName(c, referenceEntity)->showGUI({}, displayedEntities);

					ImGui::TextDisabled("Note: Editing affects all selected entities with this component");
				}
			}
			ImGui::TreePop();
		}
	}
}
