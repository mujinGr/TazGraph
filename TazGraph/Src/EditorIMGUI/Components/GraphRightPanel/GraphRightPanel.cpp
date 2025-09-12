#include "GraphRightPanel.h"

void GraphRightPanel::update(float deltaTime)
{

}

void GraphRightPanel::OnImGuiRender()
{
	ImGui::NewLine();

	int nodeCount = 0;
	int emptyCount = 0;
	int linkCount = 0;

	for (std::size_t group = Manager::groupBackgroundLayer;
		group <= Manager::buttonLabels;
		group++)
	{
		if (group == Manager::groupMinimapNodes) continue;
		// Count EmptyEntities
		auto& empties = config.c_manager->getGroup<EmptyEntity>(group);
		emptyCount += static_cast<int>(empties.size());

		// Count NodeEntities
		auto& nodes = config.c_manager->getGroup<NodeEntity>(group);
		nodeCount += static_cast<int>(nodes.size());

		// Count LinkEntities
		auto& links = config.c_manager->getGroup<LinkEntity>(group);
		linkCount += static_cast<int>(links.size());
	}

	// Show results in ImGui

	ImGui::Text("Nodes: %d", nodeCount);
	ImGui::Text("Empties: %d", emptyCount);
	ImGui::Text("Links: %d", linkCount);

	ImGui::NewLine();

	ImGui::Separator();

	ImGui::NewLine();

	if (ImGui::BeginTabBar("RightPanelTabs", ImGuiTabBarFlags_AutoSelectNewTabs)) {

		if (ImGui::BeginTabItem("ECS Groups")) {
			ShowAllEntities();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Statistics")) {
			availableFunctions();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	// popup window
	getSubcomponent<CustomFunctions>()->setSelectedEntities(config.c_selectedEntities);
	getSubcomponent<CustomFunctions>()->update();
	getSubcomponent<CustomFunctions>()->OnImGuiRender();
}

void GraphRightPanel::availableFunctions() {

	if (ImGui::Button("Calculate Degree Of Selected Entities")) {
		getSubcomponent<CustomFunctions>()->activatedScriptShown = 1;
		getSubcomponent<CustomFunctions>()->isScriptResultsOpen = true;
	}

	if (ImGui::Button("Do Signals")) {
		getSubcomponent<CustomFunctions>()->activatedScriptShown = 2;
		getSubcomponent<CustomFunctions>()->isScriptResultsOpen = true;
	}

	if (ImGui::Button("Do HeatMap")) {
		getSubcomponent<CustomFunctions>()->activatedScriptShown = 3;
		getSubcomponent<CustomFunctions>()->isScriptResultsOpen = true;
	}

	if (ImGui::Button("Do CandleStick")) {
		getSubcomponent<CustomFunctions>()->activatedScriptShown = 4;
		getSubcomponent<CustomFunctions>()->isScriptResultsOpen = true;
	}

}

template <typename TVec>
void GraphRightPanel::DrawEntityJumpList(const char* labelId, TVec& vec) {
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	if (ImGui::TreeNode(labelId)) {
		for (auto* e : vec) {
			std::string nodeLabel = "Entity ID: " + std::to_string(e->getId());
			std::string btn = "Go to##" + std::to_string(e->getId());
			if (ImGui::Button(btn.c_str())) {
				if (e->template hasComponent<TransformComponent>()) { // because clang doesnt know what type vec is
					auto& tr = e->template GetComponent<TransformComponent>();
					main_camera2D->setPosition_X(tr.getPosition().x);
					main_camera2D->setPosition_Y(tr.getPosition().y);
					main_camera2D->setAimPos(glm::vec3(main_camera2D->eyePos.x, main_camera2D->eyePos.y, main_camera2D->eyePos.z + 1.0f));
				}
			}
		}
		ImGui::TreePop();
	}
}

void GraphRightPanel::ShowAllEntities() {

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));



	ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	float size = 10;

	for (std::size_t group = Manager::groupBackgroundLayer; group != Manager::buttonLabels + 1; group++) {
		std::string s = config.c_manager->getGroupName(group);

		if (ImGui::CollapsingHeader(s.c_str())) {
			auto& nodeVec = config.c_manager->getGroup<NodeEntity>(group);
			auto& linkVec = config.c_manager->getGroup<LinkEntity>(group);
			auto& emptyVec = config.c_manager->getGroup<EmptyEntity>(group);

			// ---------- NODE GROUP ----------
			if (!nodeVec.empty()) {
				// Component Management Section for Nodes
				DrawBulkComponentControls(nodeVec, "Component", s + "_nodes");
				ImGui::Separator();
				DrawBulkComponentControls(nodeVec, "NodeComponent", s + "_nodes");

				DrawEntityJumpList(std::string("Entities##nodes_" + s).c_str(), nodeVec);

			}
			// ---------- LINK GROUP ----------
			else if (!linkVec.empty()) {
				if (ImGui::CollapsingHeader(("Component Management##links_" + s).c_str())) {
					DrawBulkComponentControls(linkVec, "LinkComponent", s + "_links");
				}

				DrawEntityJumpList(std::string("Entities##links_" + s).c_str(), linkVec);

			}
			// ---------- EMPTY GROUP ----------
			else if (!emptyVec.empty()) {
				if (ImGui::CollapsingHeader(("Component Management##empties_" + s).c_str())) {
					DrawBulkComponentControls(emptyVec, "Component", s + "_empties");
				}
				// No color/size controls by default for empties; just list them
				DrawEntityJumpList(std::string("Entities##empties_" + s).c_str(), emptyVec);
			}

		}
	}

}

template<typename EntityType>
void GraphRightPanel::DrawBulkComponentControls(const std::vector<EntityType*>& entityVec,
	const std::string& componentCategory,
	const std::string& uniqueID) {

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

	ImGui::Text("%s Components:", componentCategory.c_str());

	for (const auto& componentName : sortComponentsByID(config.c_manager->componentNames[componentCategory])) {
		// Count how many entities have this component
		int entitiesWithComponent = 0;

		EntityType* entityWithThisComponent = nullptr;


		for (auto& entity : entityVec) {
			if (entity->hasComponentByName(componentName)) {
				entitiesWithComponent++;

				if (entityWithThisComponent == nullptr)
					entityWithThisComponent = entity;
			}
		}

		// Display component info and count
		auto it = componentNameToID.find(componentName);
		if (it != componentNameToID.end()) {
			ComponentID cid = it->second;
			ImGui::Text("(ID: %u)", cid);
			ImGui::SameLine();
		}

		ImGui::Text("%s (%d/%zu)", componentName.c_str(), entitiesWithComponent, entityVec.size());
		ImGui::SameLine();

		// Add to All button
		std::string addButtonLabel = "Add to All##" + componentName + "_" + uniqueID;
		if (ImGui::Button(addButtonLabel.c_str())) {
			for (auto& entity : entityVec) {
				if (!entity->hasComponentByName(componentName)) {
					AddComponentByName(componentName, entity);
				}
			}
		}
		ImGui::SameLine();

		// Remove from All button
		std::string removeButtonLabel = "Remove from All##" + componentName + "_" + uniqueID;
		if (ImGui::Button(removeButtonLabel.c_str())) {
			for (auto& entity : entityVec) {
				if (entity->hasComponentByName(componentName)) {
					RemoveComponentByName(componentName, entity);
				}
			}
		}
		ImGui::SameLine();

		// Toggle button (adds to entities without, removes from entities with)
		std::string toggleButtonLabel = "Toggle##" + componentName + "_" + uniqueID;
		if (ImGui::Button(toggleButtonLabel.c_str())) {
			for (auto& entity : entityVec) {
				if (entity->hasComponentByName(componentName)) {
					RemoveComponentByName(componentName, entity);
				}
				else {
					AddComponentByName(componentName, entity);
				}
			}
		}

		if (entitiesWithComponent > 0) {
			BaseComponent* templateComponent = getComponentByName(componentName, entityWithThisComponent);

			if (templateComponent) {
				// Create a temporary copy for editing
					// Show the GUI for editing
				if (!templateComponent->modifyPosition)
					templateComponent->showGUI();
				else {
					std::vector<BaseComponent*> entitiesComponents = {};
					for (auto& entity : entityVec) {
						if (entity->hasComponentByName(componentName)) {
							entitiesComponents.push_back(getComponentByName(componentName, entity));
						}
					}
					templateComponent->showGUI(entitiesComponents);
					templateComponent->modifyPosition = false;
				}

			}
		}
	}
}

void GraphRightPanel::CopyComponentValues(BaseComponent* source, BaseComponent* target, size_t size) {
	std::memcpy(target, source, size);
}