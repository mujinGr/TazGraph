#include "GraphMiddlePanel.h"

void GraphMiddlePanel::OnImGuiRender()
{
	getSubcomponent<GraphTopBar>()->setConfig(
		{
			.scene = config.scene
		});
	getSubcomponent<GraphTopBar>()->OnImGuiRender();

	std::string closedTab = getSubcomponent<GraphTopBar>()->
		getTabToClose();
	if (!closedTab.empty()) {
		auto managerIt = config.scene->managers.find(closedTab);
		if (managerIt != config.scene->managers.end()) {
			config.scene->managers.erase(managerIt);

			if (!DataManager::getInstance().getMapToLoad().empty() &&
				config.scene->managers.find(DataManager::getInstance().getMapToLoad()) != config.scene->managers.end())
			{
			}
			else if (!config.scene->managers.empty())
			{
				DataManager::getInstance().setMapToLoad(config.scene->managers.begin()->first);
			}
		}
	}

	bool childActive = ImGui::BeginChild("Viewport");
	if (childActive) {

		getSubcomponent<ViewportPanel>()->setConfig({
			.c_fb = config.c_framebuffer,
			.c_minimap_fb = config.c_minimapFramebuffer,
			.c_storedWindowPos = config.c_viewportPos,
			.c_storedWindowSize = config.c_viewportSize,
			.startPos = config.startPos,
			.currPos = config.currPos,
			});
		getSubcomponent<ViewportPanel>()->OnImGuiRender();

		std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

		Manager* man = config.scene->manager;
		for (auto* textLabels : man->getGroup<EmptyEntity>(Manager::textLabels)) {

			TazGraphEngine::drawTextAtWorldPositionPerspective(
				textLabels->GetComponent<TransformComponent>().getPosition(),
				EntityIDUtils::toString(textLabels->getParentEntity()->getId()).c_str(),
				glm::vec4(1.0f),
				main_camera2D.get());

		}

	}
	ImGui::EndChild();//? Needs to be outside

	if (getSubcomponent<PythonInterpreterPanel>()) {
		getSubcomponent<PythonInterpreterPanel>()->setConfig(
			{
				.scene = config.scene,
				.viewportPos = config.c_viewportPos,
				.viewportSize = config.c_viewportSize
			}
		);
		getSubcomponent<PythonInterpreterPanel>()->OnImGuiRender();


		if (getSubcomponent<PythonInterpreterPanel>()->inputActive) {
			getSubcomponent<ViewportPanel>()->isMouseInSecondColumn = false;
		}
	}
}
