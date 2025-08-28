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

	int currentId = config.displayedEntity->getId();

	if (currentId != _lastEntityDisplayed) {
		ImGui::SetNextWindowPos(ImVec2(config.mousePos.x, config.mousePos.y), ImGuiCond_Always);
		_lastEntityDisplayed = currentId;
	}

	if (ImGui::Begin(windowTitle.c_str())) {
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

		std::unordered_map<std::string, size_t> componentOrder;
		size_t index = 0;
		for (const auto& pair : addComponentMap) {
			componentOrder[pair.first] = index++;
		}

		for (auto& [key, nameVec] : config.manager->componentNames) {
			std::sort(nameVec.begin(), nameVec.end(), [&](const std::string& a, const std::string& b) {
				return componentOrder[a] < componentOrder[b];
				});
		}

		if (node)
		{
			for (auto& c : config.manager->componentNames["Component"]) {
				// Checkbox to add/remove component
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

						if (c == "SpriteComponent") {
							std::string tempStr = config.displayedEntity->GetComponent<SpriteComponent>().texture_name;
							getComponentByName(c, config.displayedEntity)->showGUI();
							ImGui::TreePop();

							if (tempStr != config.displayedEntity->GetComponent<SpriteComponent>().texture_name) {
								config.displayedEntity->getManager()->refresh(main_camera2D.get());
							}

						}
						else {
							getComponentByName(c, config.displayedEntity)->showGUI();
							ImGui::TreePop();
						}
					}
				}
			}
			for (auto& c : config.manager->componentNames["NodeComponent"]) {
				// Checkbox to add/remove component
				bool hasComponent = config.displayedEntity->hasComponentByName(c);

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
		}


		else if (link) {
			for (auto& c : config.manager->componentNames["LinkComponent"]) {
				// Checkbox to add/remove component
				bool hasComponent = config.displayedEntity->hasComponentByName(c);

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
		}


		else if (empty) {
			for (auto& c : config.manager->componentNames["Component"]) {
				// Checkbox to add/remove component
				bool hasComponent = config.displayedEntity->hasComponentByName(c);

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

						if (c == "SpriteComponent") {
							std::string tempStr = config.displayedEntity->GetComponent<SpriteComponent>().texture_name;
							getComponentByName(c, config.displayedEntity)->showGUI();
							ImGui::TreePop();

							if (tempStr != config.displayedEntity->GetComponent<SpriteComponent>().texture_name) {
								config.displayedEntity->getManager()->refresh(main_camera2D.get());
							}

						}
						else {
							getComponentByName(c, config.displayedEntity)->showGUI();
							ImGui::TreePop();
						}
					}
				}
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