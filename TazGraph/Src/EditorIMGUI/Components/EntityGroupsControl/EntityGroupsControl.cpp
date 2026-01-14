#include "EntityGroupsControl.h"

void EntityGroupsControlPanel::OnImGuiRender()
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

	glm::vec2 mousePos = config.scene->getApp()->_inputManager.getMouseCoords();

	if (currentId != _lastEntityDisplayed) {
		ImGui::SetNextWindowPos(ImVec2(mousePos.x, mousePos.y), ImGuiCond_Always);
		_lastEntityDisplayed = currentId;
	}

	if (ImGui::Begin(windowTitle.c_str())) {

		for (auto groupElement : man->groupNames) {
			bool hasGroup = config.displayedEntity->hasGroup(groupElement.first);

			if (ImGui::Checkbox(groupElement.second.c_str(), &hasGroup)) {
				if (hasGroup) {
					config.displayedEntity->addToGroup(groupElement.first);
				}
				else {
					config.displayedEntity->removeGroup(groupElement.first);
				}
			}
		}
	}

	ImGui::End();

}
