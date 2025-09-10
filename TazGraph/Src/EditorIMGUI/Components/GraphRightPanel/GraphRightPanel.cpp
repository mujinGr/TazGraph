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
			if (ImGui::TreeNode(nodeLabel.c_str())) {
				std::string btn = "Go to##" + std::to_string(e->getId());
				if (ImGui::Button(btn.c_str())) {
					if (e->template hasComponent<TransformComponent>()) { // because clang doesnt know what type vec is
						auto& tr = e->template GetComponent<TransformComponent>();
						main_camera2D->setPosition_X(tr.getPosition().x);
						main_camera2D->setPosition_Y(tr.getPosition().y);
						main_camera2D->setAimPos(glm::vec3(main_camera2D->eyePos.x, main_camera2D->eyePos.y, main_camera2D->eyePos.z + 1.0f));
					}
				}
				ImGui::TreePop();
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
				for (auto& entity : nodeVec) { // loops 1 time
					Color initialColor = entity->GetComponent<Rectangle_w_Color>().color;
					color = ImVec4(initialColor.r / 255.0f, initialColor.g / 255.0f, initialColor.b / 255.0f, initialColor.a / 255.0f);

					TransformComponent* tr = &entity->GetComponent<TransformComponent>();
					float initialSize[2] = { tr->size.x, tr->size.y };
					size = initialSize[0];
					break;
				}

				if (ImGui::ColorEdit4(("Color##" + s).c_str(), (float*)&color)) {
					std::vector<NodeEntity*>& groupVec = config.c_manager->getGroup<NodeEntity>(group);
					Color newColor = {
					   (GLubyte)(color.x * 255),
					   (GLubyte)(color.y * 255),
					   (GLubyte)(color.z * 255),
					   (GLubyte)(color.w * 255)
					};

					for (auto& entity : groupVec) {
						entity->GetComponent<Rectangle_w_Color>().color = newColor;
					}
				}

				if (ImGui::SliderFloat("Node Size", &size, 0, 100)) {
					for (auto& entity : nodeVec) {
						entity->GetComponent<TransformComponent>().size.x = size;
						entity->GetComponent<TransformComponent>().size.y = size;
					}
				}

				ImGui::SliderFloat("Border Radius", config.c_nodeRadius, 0, 1.0f);

				DrawEntityJumpList(std::string("Entities##nodes_" + s).c_str(), nodeVec);

			}
			// ---------- LINK GROUP ----------
			else if (!linkVec.empty()) {
				for (auto& entity : linkVec) { // loops 1 time
					Color initialColor = entity->GetComponent<Line_w_Color>().src_color;
					color = ImVec4(initialColor.r / 255.0f, initialColor.g / 255.0f, initialColor.b / 255.0f, initialColor.a / 255.0f);

					break;
				}

				if (ImGui::ColorEdit4(("Color##" + s).c_str(), (float*)&color)) {
					std::vector<LinkEntity*>& groupVec = config.c_manager->getGroup<LinkEntity>(group);
					Color newColor = {
					   (GLubyte)(color.x * 255),
					   (GLubyte)(color.y * 255),
					   (GLubyte)(color.z * 255),
					   (GLubyte)(color.w * 255)
					};

					for (auto& entity : groupVec) {
						entity->GetComponent<Line_w_Color>().src_color = newColor;
						entity->GetComponent<Line_w_Color>().dest_color = newColor;
					}
				}

				DrawEntityJumpList(std::string("Entities##links_" + s).c_str(), linkVec);

			}
			// ---------- EMPTY GROUP ----------
			else if (!emptyVec.empty()) {
				// No color/size controls by default for empties; just list them
				DrawEntityJumpList(std::string("Entities##empties_" + s).c_str(), emptyVec);
			}

		}
	}

}