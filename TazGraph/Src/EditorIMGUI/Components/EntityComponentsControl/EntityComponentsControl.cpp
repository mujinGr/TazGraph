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

	glm::vec2 moysePos = config.scene->getApp()->_inputManager.getMouseCoords();

	if (currentId != _lastEntityDisplayed) {
		ImGui::SetNextWindowPos(ImVec2(moysePos.x, moysePos.y), ImGuiCond_Always);
		_lastEntityDisplayed = currentId;
	}

	if (ImGui::Begin(windowTitle.c_str())) {

		if (ImGui::CollapsingHeader("Script"))
		{
			getSubcomponent<PythonInterpreterPanel>()->setConfig({

				});

			getSubcomponent<PythonInterpreterPanel>()->OnImGuiRender2();
		}

		config.displayedEntity->imgui_print();

		if (node) {
			DataManager::getInstance().data.SetSelectData(std::move(DataManager::getInstance().pollingFileNames));

			if (ImGui::ComboAutoSelect("Select File For Polling", DataManager::getInstance().data)) {
			}
			float buttonWidth = 100;
			if (ImGui::Button("Start Polling Sending Messages", ImVec2(buttonWidth, 0))) {
				std::string selectedFile = DataManager::getInstance().data.input;
				if (!selectedFile.empty()) {
					StartPollingComponent(config.displayedEntity, selectedFile);
				}
			}
		}
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

	ImGui::End();

}


void EntityComponentsControlPanel::StartPollingComponent(Entity* entity, const std::string& fileName) {
	if (!entity) return;

	// Attach a polling component to the entity
	if (!entity->hasComponent<PollingComponent>()) {
		entity->addComponent<PollingComponent>();
	}

	entity->GetComponent<PollingComponent>().StartPolling(fileName, 10.0f);

}

void EntityComponentsControlPanel::ComponentCheckbox(std::string c) {
	bool hasComponent = config.displayedEntity->hasComponentByName(c);

	auto it = componentNameToID.find(c);
	if (it != componentNameToID.end()) {
		ComponentID cid = it->second;

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

	if (hasComponent) {
		if (ImGui::TreeNode((c + " Properties").c_str())) {
			getComponentByName(c, config.displayedEntity)->showGUI();
			ImGui::TreePop();
		}
	}
}