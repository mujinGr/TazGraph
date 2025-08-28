#include "SceneControl.h"

void SceneControlPanel::OnImGuiRender()
{
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	std::string windowTitle = "Scene Manager";

	static int sceneMan_nodeID1 = -1, sceneMan_nodeID2 = -1;
	static std::string errorMessage = "";

	ImGui::SetNextWindowPos(ImVec2(config.c_mouseCoords.x, config.c_mouseCoords.y), ImGuiCond_Always, ImVec2(0, 0));

	if (ImGui::Begin(windowTitle.c_str())) {
		if (ImGui::Button("Create Empty - Box")) {
			auto& empty(config.c_manager->addEntityNoId<Empty>());

			glm::vec2 position(0, 0);

			empty.addComponent<TransformComponent>(position, Layer::action, glm::vec3(10.0f), 1);

			empty.addComponent<BoxComponent>();

			config.c_manager->grid->addEmpty(&empty, config.c_manager->grid->getGridLevel());
			empty.addGroup(Manager::groupEmpties);
			config.c_manager->aboutTo_updateActiveEntities();
			// todo here we also have choose shape option
		}

		if (ImGui::Button("Create Empty - Plane")) {
			auto& empty(config.c_manager->addEntityNoId<Empty>());

			//empty.addGroup(config.c_manager::groupNodes_0);
			// todo here we also have choose shape option
		}

		if (ImGui::Button("Create Empty - Triangle")) {
			auto& empty(config.c_manager->addEntityNoId<Empty>());

			//empty.addGroup(config.c_manager::groupNodes_0);
			// todo here we also have choose shape option
		}

		if (ImGui::Button("Create Empty - Sphere")) {
			auto& empty(config.c_manager->addEntityNoId<Empty>());

			glm::vec2 position(0, 0);

			empty.addComponent<TransformComponent>(position, Layer::action, glm::vec3(10.0f), 1);

			empty.addComponent<SphereComponent>();

			config.c_manager->grid->addEmpty(&empty, config.c_manager->grid->getGridLevel());
			empty.addGroup(Manager::groupSphereEmpties);
			config.c_manager->aboutTo_updateActiveEntities();
		}

		ImGui::Separator();

		if (ImGui::Button("Create Node Entity")) {
			auto& node(config.c_manager->addEntityNoId<Node>());

			glm::vec2 position(0, 0);

			node.addComponent<TransformComponent>(position, Layer::action, glm::vec3(10.0f), 1);
			node.addComponent<Rectangle_w_Color>();
			node.GetComponent<Rectangle_w_Color>().color = Color(150, 150, 150, 255);

			node.GetComponent<TransformComponent>().update(0.0f); // update children positions


			config.c_manager->grid->addNode(&node, config.c_manager->grid->getGridLevel());
			node.addGroup(Manager::groupNodes_0);
			config.c_manager->aboutTo_updateActiveEntities();
		}

		ImGui::Separator();
		ImGui::Text("Create Link Entity");
		ImGui::InputInt("Node ID 1", &sceneMan_nodeID1);
		ImGui::InputInt("Node ID 2", &sceneMan_nodeID2);

		if (ImGui::Button("Create Link Entity")) {
			// Check if both nodes exist before creating the link
			if (config.c_manager->getEntityFromId(sceneMan_nodeID1) && config.c_manager->getEntityFromId(sceneMan_nodeID2)) {
				auto& link(config.c_manager->addEntityNoId<Link>(sceneMan_nodeID1, sceneMan_nodeID2));


				link.addComponent<Line_w_Color>();

				link.GetComponent<Line_w_Color>().setSrcColor(Color(255, 40, 0, 255));
				link.GetComponent<Line_w_Color>().setDestColor(Color(40, 255, 0, 255));

				link.addComponent<LineFlashAnimatorComponent>();


				link.addGroup(Manager::groupLinks_0);
				config.c_manager->grid->addLink(&link, config.c_manager->grid->getGridLevel());
				config.c_manager->aboutTo_updateActiveEntities();
				errorMessage = ""; // Clear error if successful
			}
			else {
				errorMessage = "Error: One or both node IDs do not exist!";
			}
		}
	}

	ImGui::End();
}
