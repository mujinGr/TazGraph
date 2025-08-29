#include "GraphRightPanel.h"

void GraphRightPanel::update(float deltaTime)
{

}

void GraphRightPanel::OnImGuiRender()
{
	availableFunctions();
	ImGui::Separator();
	ShowAllEntities();

	_customFunctions.setSelectedEntities(config.c_selectedEntities);
	_customFunctions.update();
	_customFunctions.OnImGuiRender();
}

void GraphRightPanel::availableFunctions() {

	if (ImGui::Button("Calculate Degree Of Selected Entities")) {
		_customFunctions.activatedScriptShown = 1;
	}

	if (ImGui::Button("Do Signals")) {
		_customFunctions.activatedScriptShown = 2;
	}

	if (ImGui::Button("Do HeatMap")) {
		_customFunctions.activatedScriptShown = 3;
	}

	if (ImGui::Button("Do CandleStick")) {
		_customFunctions.activatedScriptShown = 4;
	}

	if (ImGui::Button("Show Script Results")) {
		_customFunctions.isScriptResultsOpen = !_customFunctions.isScriptResultsOpen;
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
			std::vector<NodeEntity*>& groupVec = config.c_manager->getGroup<NodeEntity>(group);

			if (group == Manager::groupNodes_0 || group == Manager::groupGroupNodes_0 || group == Manager::groupGroupNodes_1) {

				for (auto& entity : groupVec) { // loops 1 time
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
					for (auto& entity : groupVec) {
						entity->GetComponent<TransformComponent>().size.x = size;
						entity->GetComponent<TransformComponent>().size.y = size;
					}
				}

				ImGui::SliderFloat("Border Radius", config.c_nodeRadius, 0, 1.0f);

			}

			if (group == Manager::groupLinks_0 || group == Manager::groupGroupLinks_0 || group == Manager::groupGroupLinks_1) {
				for (auto& entity : groupVec) { // loops 1 time
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

				/*if (ImGui::SliderInt("Line Width", &size, 0, 100)) {
					for (auto& entity : groupVec) {
						entity->GetComponent<TransformComponent>().width = size;
						entity->GetComponent<TransformComponent>().height = size;
					}
				}*/
			}


			std::string treeNodeLabel = "Entities##" + config.c_manager->getGroupName(group);
			if (ImGui::TreeNode(treeNodeLabel.c_str())) {

				for (auto& entity : groupVec) {

					std::string label = "Entity ID: " + std::to_string(entity->getId());

					if (ImGui::TreeNode(label.c_str())) {

						// Create a unique button label
						std::string buttonLabel = "Go to##" + std::to_string(entity->getId());
						if (ImGui::Button(buttonLabel.c_str())) {
							if (entity->hasComponent<TransformComponent>()) {
								main_camera2D->setPosition_X(entity->GetComponent<TransformComponent>().getPosition().x);
								main_camera2D->setPosition_Y(entity->GetComponent<TransformComponent>().getPosition().y);

								main_camera2D->setAimPos(glm::vec3(main_camera2D->eyePos.x, main_camera2D->eyePos.y, main_camera2D->eyePos.z + 1.0f));
							}
						}

						ImGui::TreePop();
					}

				}
				ImGui::TreePop();
			}

		}
	}

}