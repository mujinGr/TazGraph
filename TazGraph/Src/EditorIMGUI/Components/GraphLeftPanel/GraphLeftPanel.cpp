#include "GraphLeftPanel.h"

void GraphLeftPanel::update(float deltaTime) {
	UIElement::update(deltaTime);

	if (last_activeLayout < activeLayout) {
		last_activeLayout += 1;

		config.scene->manager->grid->setGridLevel(static_cast<Grid::Level>(config.scene->manager->grid->getGridLevel() + 1));

		if (config.scene->manager->grid->getGridLevel() == Grid::Level::Outer1) {
			AssetManager::createGroupLayout(config.scene->manager, Grid::Level::Outer1);
		}
		else if (config.scene->manager->grid->getGridLevel() == Grid::Level::Outer2) {
			AssetManager::createGroupLayout(config.scene->manager, Grid::Level::Outer2);
		}
	}

	if (last_activeLayout > activeLayout) {
		last_activeLayout -= 1;

		if (config.scene->manager->grid->getGridLevel() == Grid::Level::Outer1) {
			AssetManager::ungroupLayout(config.scene->manager, Grid::Level::Outer1);
		}
		else if (config.scene->manager->grid->getGridLevel() == Grid::Level::Outer2) {
			AssetManager::ungroupLayout(config.scene->manager, Grid::Level::Outer2);
		}

		config.scene->manager->grid->setGridLevel(static_cast<Grid::Level>(last_activeLayout));
	}
}

void GraphLeftPanel::OnImGuiRender()
{
	if (ImGui::BeginChild("Background UI")) {
		ImVec4 activeColor = ImVec4(0.2f, 0.7f, 0.2f, 1.0f);
		ImVec4 inactiveColor = ImVec4(0.7f, 0.2f, 0.2f, 1.0f);

		ImVec4 defaultColor = ImVec4(0.0f, 0.5f, 1.0f, 1.0f);

		glm::vec2 mousePos = config.scene->getApp()->_inputManager.getMouseCoords();

		// Change color based on the debug mode state
		if (config.scene->renderDebug) {
			ImGui::PushStyleColor(ImGuiCol_Button, activeColor);  // Green for ON
		}
		else {
			ImGui::PushStyleColor(ImGuiCol_Button, inactiveColor);  // Red for OFF
		}
		// Button toggles the debug mode
		if (ImGui::Button("Enable Debug Mode")) {
			config.scene->renderDebug = !config.scene->renderDebug;  // Toggle the state
		}
		ImGui::PopStyleColor(1);

		if (config.scene->manager->idTextEnabled) {
			ImGui::PushStyleColor(ImGuiCol_Button, activeColor);  // Green for ON
		}
		else {
			ImGui::PushStyleColor(ImGuiCol_Button, inactiveColor);  // Red for OFF
		}
		// Button toggles the debug mode
		if (ImGui::Button(config.scene->manager->idTextEnabled ? "Disable Id Labels" : "Enable Id Labels")) {
			config.scene->manager->idTextEnabled = !config.scene->manager->idTextEnabled;

			if (config.scene->manager->idTextEnabled) {
				//create empty entities(textLabels) that will attach on node entities
				for (auto nodeId : config.scene->manager->getGroup<NodeEntity>(Manager::groupNodes_0)) {
					auto* node = config.scene->manager->getEntityFromId(nodeId);

					auto& textLabel = config.scene->manager->addEntity<Empty>();

					textLabel.addGroup(Manager::textLabels);

					textLabel.addComponent<TransformComponent>(0.0f);
					textLabel.GetComponent<TransformComponent>().local_position = glm::vec3(0);
					node->children["label"] = textLabel.getId();
					config.scene->manager->getEntityFromId(node->children["label"])->setParentEntity(node, "label");
					config.scene->manager->getEntityFromId(node->children["label"])->GetComponent<TransformComponent>().initChild();
				}
			}
			else if (!config.scene->manager->idTextEnabled) {
				//destroy empty entities(textLabels) 
				config.scene->manager->removeAllEntitiesFromEmptyGroup(Manager::textLabels);

				for (auto& labelId : config.scene->manager->getGroup<EmptyEntity>(Manager::textLabels)) {
					auto* label = config.scene->manager->getEntityFromId(labelId);
					auto& node = *label->getParentEntity();

					node.children.erase("label");
				}
			}
			config.scene->manager->aboutTo_updateActiveEntities();
		}
		ImGui::PopStyleColor(1);

		ImGui::Separator();

		if (ImGui::Button(config.scene->showGrid ? "Hide Grid" : "Show Grid")) {
			config.scene->showGrid = !config.scene->showGrid;
		}

		if (ImGui::Button(config.scene->manager->arrowheadsEnabled ? "Disable Arrowheads" : "Enable Arrowheads")) {
			config.scene->manager->arrowheadsEnabled = !config.scene->manager->arrowheadsEnabled;
			config.scene->manager->updateInnerPathLinks = true;
			//manager.setArrowheadsEnabled(arrowheadsEnabled); // Call function to apply change
		}

		ImGui::Separator();

		ImGui::Text("Select Grouping Level:");
		if (ImGui::RadioButton("1", activeLayout == 0)) {
			activeLayout = 0;
		}

		ImGui::SameLine();
		if (ImGui::RadioButton("2", activeLayout == 1)) {
			activeLayout = 1;
		}

		ImGui::SameLine();
		if (ImGui::RadioButton("3", activeLayout == 2)) {
			activeLayout = 2;
		}

		ImGui::Separator();



		ImGui::Text("Grid TazSize: %u", config.scene->manager->grid->getCellSize());


		ImGui::Separator();


		ImGui::Text("Scene/Screen Coords: {x: %f, y: %f}", config.sceneMouseCoords.x, config.sceneMouseCoords.y);
		ImGui::Text("MainViewport Coords: {x: %f, y: %f}", mousePos.x, mousePos.y);


		ImGui::Separator();

		DataManager::getInstance().pathData.SetSelectData(std::move(DataManager::getInstance().pathsFileNames));

		if (ImGui::ComboAutoSelect("Choose Links Path File", DataManager::getInstance().pathData)) {
			std::string resetIndex = ">Reset";
			if (strcmp(DataManager::getInstance().pathData.input, resetIndex.c_str()) == 0) {

				for (auto pathHolderId : config.scene->manager->getGroup<EmptyEntity>(Manager::groupPathLinksHolder)) {
					auto* pathHolder = config.scene->manager->getEntityFromId(pathHolderId);
					auto& pathLinks = pathHolder->GetComponent<PathLinkerComponent>().pathLinks;

					for (auto linkId : pathLinks) {
						Entity* link = config.scene->manager->getEntityFromId(linkId);
						link->destroy();
					}
					config.scene->manager->updateInnerPathLinks = true;
				}
				config.scene->manager->removeAllEntitiesFromEmptyGroup(Manager::groupPathLinksHolder);
			}
			else {
				DataManager::getInstance().pathLoading = DataManager::getInstance().pathData.input;
				DataManager::getInstance().setPathLoading(true);
			}

		}

		ImGui::Separator();

		if (ImGui::BeginTabBar("LeftPanelTabs", ImGuiTabBarFlags_AutoSelectNewTabs)) {

			if (ImGui::BeginTabItem("Layout")) {

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ChooseLayoutPanel();

		ImGui::Separator();

		if (config.c_selectedEntities.size() == 1) {
			displayChildrenRecursive(config.c_selectedEntities.front().first, 0);
		}

		ImGui::EndChild();
	}
}


void GraphLeftPanel::ChooseLayoutPanel() {
	ImVec4 activeColor = ImVec4(0.2f, 0.7f, 0.2f, 1.0f);
	ImVec4 inactiveColor = ImVec4(0.7f, 0.2f, 0.2f, 1.0f);

	ImVec4 defaultColor = ImVec4(0.0f, 0.5f, 1.0f, 1.0f);

	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
	ImGui::Text("Circular");

	ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 24);

	if (ImGui::ImageButton("Circular", static_cast<ImTextureID>(static_cast<intptr_t>(TextureManager::getInstance().Get_GLTexture("play-button")->id)), ImVec2(16, 16))) {

		auto& nodes = config.scene->manager->getGroup<NodeEntity>(Manager::groupNodes_0);
		if (nodes.empty()) return;

		NodeEntity* centerNode = nullptr;
		int maxOutlinks = -1;

		for (EntityID nodeId : nodes) {
			auto* node = dynamic_cast<NodeEntity*>(config.scene->manager->getEntityFromId(nodeId));
			int outLinks = node->getOutLinks().size();
			if (outLinks > maxOutlinks) {
				maxOutlinks = outLinks;
				centerNode = node;
			}
		}

		if (!centerNode) return;

		glm::vec2 centerPos = glm::vec2(0.0f, 0.0f);
		centerNode->GetComponent<TransformComponent>().setPosition_X(centerPos.x);
		centerNode->GetComponent<TransformComponent>().setPosition_Y(centerPos.y);

		float minRadius = 100.0f;
		float maxRadius = 3000.0f;

		int minOutlinks = INT_MAX;
		for (EntityID nodeId : nodes) {
			if (nodeId == centerNode->getId()) continue;

			auto* node = dynamic_cast<NodeEntity*>(config.scene->manager->getEntityFromId(nodeId));

			int count = node->getOutLinks().size();

			minOutlinks = std::min(minOutlinks, count);
			maxOutlinks = std::max(maxOutlinks, count);

		}


		size_t index = 0;
		size_t total = nodes.size() - 1;

		for (EntityID nodeId : nodes) {
			if (nodeId == centerNode->getId()) continue;
			auto* node = dynamic_cast<NodeEntity*>(config.scene->manager->getEntityFromId(nodeId));

			float angle = (2 * M_PI * index) / total;
			int outLinks = node->getOutLinks().size();

			float normalized = (float)(maxOutlinks - outLinks) / std::max(1, maxOutlinks - minOutlinks);
			float radius = minRadius + normalized * (maxRadius - minRadius);

			glm::vec2 pos = centerPos + glm::vec2(cos(angle), sin(angle)) * radius;

			node->GetComponent<TransformComponent>().setPosition_X(pos.x);
			node->GetComponent<TransformComponent>().setPosition_Y(pos.y);


			++index;
		}

		config.scene->manager->aboutTo_updateActiveEntities();

		for (auto& nId : config.scene->manager->getGroup<NodeEntity>(Manager::groupNodes_0))
		{
			auto* n =
				dynamic_cast<NodeEntity*>(
					config.scene->manager->getEntityFromId(nId)
					);

			n->cellUpdate();
		}
		for (auto& nId : config.scene->manager->getGroup<NodeEntity>(Manager::groupGroupNodes_0))
		{
			auto* n =
				dynamic_cast<NodeEntity*>(
					config.scene->manager->getEntityFromId(nId)
					);
			n->cellUpdate();
		}
		for (auto& nId : config.scene->manager->getGroup<NodeEntity>(Manager::groupGroupNodes_1))
		{
			auto* n =
				dynamic_cast<NodeEntity*>(
					config.scene->manager->getEntityFromId(nId)
					);
			n->cellUpdate();
		}
		for (auto& lId : config.scene->manager->getGroup<LinkEntity>(Manager::groupLinks_0))
		{
			auto* l =
				dynamic_cast<LinkEntity*>(
					config.scene->manager->getEntityFromId(lId)
					);
			l->cellUpdate();
		}
		for (auto& lId : config.scene->manager->getGroup<LinkEntity>(Manager::groupGroupLinks_0))
		{
			auto* l =
				dynamic_cast<LinkEntity*>(
					config.scene->manager->getEntityFromId(lId)
					);
			l->cellUpdate();
		}
		for (auto& lId : config.scene->manager->getGroup<LinkEntity>(Manager::groupGroupLinks_1))
		{
			auto* l =
				dynamic_cast<LinkEntity*>(
					config.scene->manager->getEntityFromId(lId)
					);
			l->cellUpdate();
		}
	}

	ImGui::Separator();

	ImGui::Text("Cluster");

	ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 24);

	if (_clusterLayout) {
		ImGui::PushStyleColor(ImGuiCol_Button, activeColor);  // Green for ON
	}
	else {
		ImGui::PushStyleColor(ImGuiCol_Button, inactiveColor);  // Red for OFF
	}

	if (ImGui::ImageButton("Cluster", static_cast<ImTextureID>(static_cast<intptr_t>(TextureManager::getInstance().Get_GLTexture("play-button")->id)), ImVec2(16, 16))) {

		auto clusterGroupLayout = [&](Group nodeGroup, Group linkGroup)
			{
				auto& nodes = config.scene->manager->getGroup<NodeEntity>(nodeGroup);
				auto& links = config.scene->manager->getGroup<LinkEntity>(linkGroup);

				if (_clusterLayout) {
					/*for (NodeEntity* node : nodes) {
						node->addGroup(config.scene->manager::groupColliders);
					}*/

					for (EntityID nodeId : nodes) {
						auto* node =
							dynamic_cast<NodeEntity*>(
								config.scene->manager->getEntityFromId(nodeId)
								);

						node->addGroup(Manager::groupColliders);
						node->addComponent<ColliderComponent>(
							config.scene->manager,
							node->GetComponent<TransformComponent>().size);

						node->GetComponent<ColliderComponent>().addCollisionGroup(nodeGroup);
					}

					for (EntityID linkId : links) {
						auto* link =
							dynamic_cast<LinkEntity*>(
								config.scene->manager->getEntityFromId(linkId)
								);

						link->addComponent<SpringComponent>();
					}
				}
				else {
					for (EntityID nodeId : nodes) {
						auto* node =
							dynamic_cast<NodeEntity*>(
								config.scene->manager->getEntityFromId(nodeId)
								);
						if (node->hasComponent<ColliderComponent>()) {
							node->removeGroup(Manager::groupColliders);
							node->removeComponent<ColliderComponent>();
						}
					}
					for (EntityID linkId : links) {
						auto* link =
							dynamic_cast<LinkEntity*>(
								config.scene->manager->getEntityFromId(linkId)
								);
						if (link->hasComponent<SpringComponent>()) {
							link->removeComponent<SpringComponent>();
						}
					}
				}
			};

		_clusterLayout = !_clusterLayout;

		clusterGroupLayout(Manager::groupNodes_0, Manager::groupLinks_0);
		clusterGroupLayout(Manager::groupGroupNodes_0, Manager::groupGroupLinks_0);
		clusterGroupLayout(Manager::groupGroupNodes_1, Manager::groupGroupLinks_1);

		config.scene->manager->aboutTo_updateActiveEntities();

	}
	ImGui::PopStyleColor(1);
	ImGui::PopStyleVar();
}

void GraphLeftPanel::displayChildrenRecursive(EntityID entityId, int depth)
{
	Entity* entity = config.scene->manager->getEntityFromId(entityId);

	if (!entity) return;

	ImGui::Text("Selected Entity:");

	std::string entityIdStr = EntityIDUtils::toString(entity->getId());
	std::string label = "ID: " + entityIdStr;

	ImGuiTreeNodeFlags baseFlags =
		ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_SpanAvailWidth;

	if (entity->children.empty())
		baseFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	ImGui::PushID(entityIdStr.c_str());

	bool nodeOpen = ImGui::TreeNodeEx(label.c_str(), baseFlags);

	if (nodeOpen && !entity->children.empty()) {
		for (auto& [childId, child] : entity->children) {
			auto* childEnt = config.scene->manager->getEntityFromId(child);

			if (!childEnt) continue;

			// Convert ID to string
			std::string childIdStr = EntityIDUtils::toString(childId);

			// Print both index and entity ID
			std::string childLabel = "[" + childIdStr + "] - " + EntityIDUtils::toString(childEnt->getId());

			ImGuiTreeNodeFlags childFlags =
				ImGuiTreeNodeFlags_OpenOnArrow |
				ImGuiTreeNodeFlags_OpenOnDoubleClick |
				ImGuiTreeNodeFlags_SpanAvailWidth;

			if (childEnt->children.empty())
				childFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

			ImGui::PushID(childIdStr.c_str());

			bool childOpen = ImGui::TreeNodeEx(childLabel.c_str(), childFlags);

			if (childOpen && !childEnt->children.empty()) {
				displayChildrenRecursive(child, depth + 1);
				ImGui::TreePop();
			}

			ImGui::PopID();
		}
		ImGui::TreePop();
	}

	ImGui::PopID();
}