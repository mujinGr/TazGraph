#include "GraphLeftPanel.h"

void GraphLeftPanel::update(float deltaTime) {
	sliderRotate.update(deltaTime);

	if (last_activeLayout < activeLayout) {
		last_activeLayout += 1;

		config.manager->grid->setGridLevel(static_cast<Grid::Level>(config.manager->grid->getGridLevel() + 1));

		if (config.manager->grid->getGridLevel() == Grid::Level::Outer1) {
			AssetManager::createGroupLayout(config.manager, Grid::Level::Outer1);
		}
		else if (config.manager->grid->getGridLevel() == Grid::Level::Outer2) {
			AssetManager::createGroupLayout(config.manager, Grid::Level::Outer2);
		}
	}

	if (last_activeLayout > activeLayout) {
		last_activeLayout -= 1;

		if (config.manager->grid->getGridLevel() == Grid::Level::Outer1) {
			AssetManager::ungroupLayout(config.manager, Grid::Level::Outer1);
		}
		else if (config.manager->grid->getGridLevel() == Grid::Level::Outer2) {
			AssetManager::ungroupLayout(config.manager, Grid::Level::Outer2);
		}

		config.manager->grid->setGridLevel(static_cast<Grid::Level>(config.manager->grid->getGridLevel() - 1));
	}
}

void GraphLeftPanel::OnImGuiRender()
{
	ImGui::BeginChild("Background UI");

	ImVec4 activeColor = ImVec4(0.2f, 0.7f, 0.2f, 1.0f);
	ImVec4 inactiveColor = ImVec4(0.7f, 0.2f, 0.2f, 1.0f);

	ImVec4 defaultColor = ImVec4(0.0f, 0.5f, 1.0f, 1.0f);

	ImGui::Separator();

	ImGui::Text("Camera:");

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	ImGui::Text("Rect: {x: %f, y: %f, w: %f, h: %f}", main_camera2D->getCameraRect().x, main_camera2D->getCameraRect().y, main_camera2D->getCameraRect().w, main_camera2D->getCameraRect().h);

	if (ImGui::SliderFloat3("Eye Position", &main_camera2D->eyePos[0], -1000.0f, 1000.0f)) {
		main_camera2D->setCameraMatrix(glm::lookAt(main_camera2D->eyePos, main_camera2D->aimPos, main_camera2D->upDir));
	}
	if (ImGui::SliderFloat3("Aim Position", &main_camera2D->aimPos[0], -1000.0f, 1000.0f)) {
		main_camera2D->setCameraMatrix(glm::lookAt(main_camera2D->eyePos, main_camera2D->aimPos, main_camera2D->upDir));
	}
	if (ImGui::SliderFloat3("Up Direction", &main_camera2D->upDir[0], -1000.0f, 1000.0f)) {
		main_camera2D->setCameraMatrix(glm::lookAt(main_camera2D->eyePos, main_camera2D->aimPos, main_camera2D->upDir));
	}

	sliderRotate.OnImGuiRender();

	const char* viewModeNames[] = { "Y-Up", "Z-Up" };

	if (ImGui::Combo("Orientation", &_currentOrientationIndex, viewModeNames, IM_ARRAYSIZE(viewModeNames))) {
		ViewMode newMode = static_cast<ViewMode>(_currentOrientationIndex);
		main_camera2D->currentViewMode = newMode;

		glm::vec3 eyePos = main_camera2D->getPosition();

		if (newMode == ViewMode::Y_UP) {
			//main_camera2D->upDir = glm::vec3(0, -1, 0);
			main_camera2D->setAimPos(glm::vec3(eyePos.x, eyePos.y, eyePos.z + 1.0f));
		}
		else if (newMode == ViewMode::Z_UP) {
			//main_camera2D->upDir = glm::vec3(0, 0, -1);
			main_camera2D->setAimPos(glm::vec3(eyePos.x, eyePos.y + 1.0f, eyePos.z));
		}

	}

	if (ImGui::Button("Reset")) {
		main_camera2D->resetCameraPosition();
	}

	ImGui::Separator();
	// Change color based on the debug mode state
	if (*config.renderDebug) {
		ImGui::PushStyleColor(ImGuiCol_Button, activeColor);  // Green for ON
	}
	else {
		ImGui::PushStyleColor(ImGuiCol_Button, inactiveColor);  // Red for OFF
	}
	// Button toggles the debug mode
	if (ImGui::Button("Enable Debug Mode")) {
		*config.renderDebug = !*config.renderDebug;  // Toggle the state
	}

	ImGui::PopStyleColor(1);

	ImGui::Separator();

	if (ImGui::Button(config.manager->arrowheadsEnabled ? "Disable Arrowheads" : "Enable Arrowheads")) {
		config.manager->arrowheadsEnabled = !config.manager->arrowheadsEnabled;
		config.manager->updateInnerPathLinks = true;
		//manager.setArrowheadsEnabled(arrowheadsEnabled); // Call function to apply change
	}

	ImGui::Separator();

	ImGui::Text("Select Grouping Layout:");
	ImGui::PushStyleColor(ImGuiCol_Button, activeLayout == 0 ? activeColor : defaultColor);
	if (ImGui::Button("Default Layout", ImVec2(120, 30))) {
		activeLayout = 0;
	}
	ImGui::PopStyleColor();

	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Button, activeLayout == 1 ? activeColor : defaultColor);
	if (ImGui::Button("Group Layout 1", ImVec2(120, 30))) {
		activeLayout = 1;
	}
	ImGui::PopStyleColor();

	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Button, activeLayout == 2 ? activeColor : defaultColor);
	if (ImGui::Button("Group Layout 2", ImVec2(120, 30))) {
		activeLayout = 2;
	}
	ImGui::PopStyleColor();

	ImGui::Separator();

	ImGui::Text("Choose Layout:");

	if (ImGui::Button("Circular", ImVec2(120, 30))) {

		auto& nodes = config.manager->getGroup<NodeEntity>(Manager::groupNodes_0);
		if (nodes.empty()) return;

		NodeEntity* centerNode = nullptr;
		int maxOutlinks = -1;

		for (NodeEntity* node : nodes) {
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
		for (NodeEntity* node : nodes) {
			if (node == centerNode) continue;
			int count = node->getOutLinks().size();

			minOutlinks = std::min(minOutlinks, count);
			maxOutlinks = std::max(maxOutlinks, count);

		}


		size_t index = 0;
		size_t total = nodes.size() - 1;

		for (NodeEntity* node : nodes) {
			if (node == centerNode) continue;

			float angle = (2 * M_PI * index) / total;
			int outLinks = node->getOutLinks().size();

			float normalized = (float)(maxOutlinks - outLinks) / std::max(1, maxOutlinks - minOutlinks);
			float radius = minRadius + normalized * (maxRadius - minRadius);

			glm::vec2 pos = centerPos + glm::vec2(cos(angle), sin(angle)) * radius;

			node->GetComponent<TransformComponent>().setPosition_X(pos.x);
			node->GetComponent<TransformComponent>().setPosition_Y(pos.y);


			++index;
		}

		config.manager->aboutTo_updateActiveEntities();

		for (auto& n : config.manager->getGroup<NodeEntity>(Manager::groupNodes_0))
		{
			n->cellUpdate();
		}
		for (auto& n : config.manager->getGroup<NodeEntity>(Manager::groupGroupNodes_0))
		{
			n->cellUpdate();
		}
		for (auto& n : config.manager->getGroup<NodeEntity>(Manager::groupGroupNodes_1))
		{
			n->cellUpdate();
		}
		for (auto& l : config.manager->getGroup<LinkEntity>(Manager::groupLinks_0))
		{
			l->cellUpdate();
		}
		for (auto& l : config.manager->getGroup<LinkEntity>(Manager::groupGroupLinks_0))
		{
			l->cellUpdate();
		}
		for (auto& l : config.manager->getGroup<LinkEntity>(Manager::groupGroupLinks_1))
		{
			l->cellUpdate();
		}
	}

	if (_clusterLayout) {
		ImGui::PushStyleColor(ImGuiCol_Button, activeColor);  // Green for ON
	}
	else {
		ImGui::PushStyleColor(ImGuiCol_Button, inactiveColor);  // Red for OFF
	}

	if (ImGui::Button(_clusterLayout ? "Disable Cluster" : "Cluster", ImVec2(120, 30))) {

		auto clusterGroupLayout = [&](Group nodeGroup, Group linkGroup)
			{
				auto& nodes = config.manager->getGroup<NodeEntity>(nodeGroup);
				auto& links = config.manager->getGroup<LinkEntity>(linkGroup);

				if (_clusterLayout) {
					/*for (NodeEntity* node : nodes) {
						node->addGroup(config.manager::groupColliders);
					}*/

					for (NodeEntity* node : nodes) {
						node->addGroup(Manager::groupColliders);
						node->addComponent<ColliderComponent>(
							config.manager,
							node->GetComponent<TransformComponent>().size);

						node->GetComponent<ColliderComponent>().addCollisionGroup(nodeGroup);
					}

					for (LinkEntity* link : links) {
						link->addComponent<SpringComponent>();
					}
				}
				else {
					for (NodeEntity* node : nodes) {
						if (node->hasComponent<ColliderComponent>()) {
							node->removeGroup(Manager::groupColliders);
							node->removeComponent<ColliderComponent>();
						}
					}
					for (LinkEntity* link : links) {
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

		config.manager->aboutTo_updateActiveEntities();

	}
	ImGui::PopStyleColor(1);

	ImGui::Separator();

	DataManager::getInstance().pathData.SetSelectData(std::move(DataManager::getInstance().pathsFileNames));

	if (ImGui::ComboAutoSelect("Choose Links Path File", DataManager::getInstance().pathData)) {
		std::string resetIndex = ">Reset";
		if (strcmp(DataManager::getInstance().pathData.input, resetIndex.c_str()) == 0) {

			for (auto& pathHolder : config.manager->getGroup<EmptyEntity>(Manager::groupPathLinksHolder)) {
				auto& pathLinks = pathHolder->GetComponent<PathLinkerComponent>().pathLinks;

				for (auto* link : pathLinks) {
					NodeEntity* from = link->getFromNode();
					NodeEntity* to = link->getToNode();

					if (from) {
						from->removeOutLink(link);
						from->removeSlots(); // if slots are per-link
					}
					if (to) {
						to->removeInLink(link);
						to->removeSlots();
					}

					link->removeArrowHead();
					link->resetPorts();
				}
			}
			config.manager->removeAllEntitiesFromEmptyGroup(Manager::groupPathLinksHolder);

			// remove related links
			config.manager->removeAllEntitiesFromLinkGroup(Manager::groupPathInnerLinks);

			config.manager->removeAllEntitiesFromLinkGroup(Manager::groupPathLinks);
		}
		else {
			DataManager::getInstance().pathLoading = DataManager::getInstance().pathData.input;
			DataManager::getInstance().setPathLoading(true);
		}

	}

	ImGui::Separator();

	if (ImGui::BeginTable("GroupsTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
		ImGui::TableSetupColumn("Group Name", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Entity Count", ImGuiTableColumnFlags_WidthFixed, 100.0f);
		ImGui::TableHeadersRow();

		int totalEntities = 0;
		for (std::size_t managerGroup = Manager::groupBackgroundLayer; managerGroup != Manager::buttonLabels + 1; ++managerGroup) {
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("%s", config.manager->getGroupName(managerGroup).c_str());
			ImGui::TableSetColumnIndex(1);
			int groupSize = config.manager->getVisibleGroup<EmptyEntity>(managerGroup).size() + config.manager->getVisibleGroup<NodeEntity>(managerGroup).size() + config.manager->getVisibleGroup<LinkEntity>(managerGroup).size();
			ImGui::Text("%d", groupSize);

			totalEntities += groupSize;
		}

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Total Visible Entities");
		ImGui::TableSetColumnIndex(1);
		ImGui::Text("%d", totalEntities);

		ImGui::EndTable();
	}

	if (ImGui::BeginTable("GroupsTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
		ImGui::TableSetupColumn("Vectors", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Size()", ImGuiTableColumnFlags_WidthFixed, 100.0f);
		ImGui::TableHeadersRow();

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("entities");
		ImGui::TableSetColumnIndex(1);
		ImGui::Text("%zu", config.manager->getEntities().size());

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("visible empty entities");
		ImGui::TableSetColumnIndex(1);
		ImGui::Text("%zu", config.manager->getVisible<EmptyEntity>().size());

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("visible nodes");
		ImGui::TableSetColumnIndex(1);
		ImGui::Text("%zu", config.manager->getVisible<NodeEntity>().size());

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("visible links");
		ImGui::TableSetColumnIndex(1);
		ImGui::Text("%zu", config.manager->getVisible<LinkEntity>().size());

		ImGui::EndTable();
	}

	ImGui::Text("Grid Size: %u", config.manager->grid->getCellSize());


	ImGui::Separator();


	ImGui::Text("Scene/Screen Coords: {x: %f, y: %f}", config.sceneMouseCoords.x, config.sceneMouseCoords.y);
	ImGui::Text("MainViewport Coords: {x: %f, y: %f}", config.mouseCoords.x, config.mouseCoords.y);




	ImGui::EndChild();
}
