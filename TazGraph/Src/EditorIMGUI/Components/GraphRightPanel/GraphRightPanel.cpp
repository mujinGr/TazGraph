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

	for (auto group : config.scene->manager->groupNames)
	{
		if (group.first == Manager::groupMinimapNodes) continue;
		// Count EmptyEntities
		auto& empties = config.scene->manager->getGroup<EmptyEntity>(group.first);
		emptyCount += static_cast<int>(empties.size());

		// Count NodeEntities
		auto& nodes = config.scene->manager->getGroup<NodeEntity>(group.first);
		nodeCount += static_cast<int>(nodes.size());

		// Count LinkEntities
		auto& links = config.scene->manager->getGroup<LinkEntity>(group.first);
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
			ShowGroupComponents();
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
	getSubcomponent<CustomFunctions>()->setConfig({ .scene = config.scene });
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

void GraphRightPanel::ShowGroupComponents() {

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));



	ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	float size = 10;

	ImGui::Text("Selected Group:");

	if (ImGui::RadioButton("(None)", &selectedGroup, -1)) {

	}

	if (ImGui::CollapsingHeader("##Groups")) {
		for (auto groupElement : config.scene->manager->groupNames) {
			auto group = groupElement.first;
			std::string s = config.scene->manager->getGroupName(group);

			if (ImGui::RadioButton(s.c_str(), &selectedGroup, group)) {

			}
		}
	}

	ImGui::Separator();
	if (config.c_selectedEntities.size() == 1) {

		Entity* entity = config.scene->manager->getEntityFromId(config.c_selectedEntities[0].realEntityId);

		getSubcomponent<EntityComponentsControlPanel>()->setConfig({
			.scene = config.scene,
			.displayedEntity = entity
			});
		getSubcomponent<EntityComponentsControlPanel>()->OnImGuiRender();
	}
}

void GraphRightPanel::CopyComponentValues(BaseComponent* source, BaseComponent* target, size_t size) {
	std::memcpy(target, source, size);
}