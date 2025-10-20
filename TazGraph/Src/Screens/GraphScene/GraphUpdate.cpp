#include "Graph.h"
#include <AppScene/AppInterface.h>

void Graph::update(float deltaTime) //game objects updating
{
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));
	std::shared_ptr<OrthoCamera> minimap_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("minimap"));

	main_camera2D->update();
	hud_camera2D->update();
	minimap_camera2D->update();

	if (!manager) {
		return;
	}
	manager->refresh(main_camera2D.get());

	/*glm::vec3 cameraAimPos = main_camera2D->getAimPos();

	glm::vec3 directionToCamera = glm::normalize(cameraAimPos - main_camera2D->eyePos);

	glm::vec3 cameraEulerAngles = main_camera2D->getEulerAnglesFromDirection(directionToCamera);
		*/
	if (_firstLoop) {
		manager->updateFully(deltaTime);
	}
	else {
		manager->update(deltaTime);
	}

	for (auto nodeId : manager->getVisibleGroup<NodeEntity>(Manager::groupColliders)) {
		auto* node = dynamic_cast<NodeEntity*>(manager->getEntityFromId(nodeId));

		node->GetComponent<ColliderComponent>().collisionPhysics();
	}

	/*

	action: add path links

	-> if arrowheads are enabled,



	*/

	if ((manager->last_arrowheadsEnabled != manager->arrowheadsEnabled)
		&& manager->arrowheadsEnabled) {
		//todo add to all nodes ports
		for (auto nodeId : manager->getGroup<NodeEntity>(manager->grid->getGridLevel() == Grid::Level::Basic ? Manager::groupNodes_0 :
			(manager->grid->getGridLevel() == Grid::Level::Outer1 ? Manager::groupGroupNodes_0 :
				Manager::groupGroupNodes_1
				)))
		{
			auto* node = dynamic_cast<NodeEntity*>(manager->getEntityFromId(nodeId));
			node->addPorts();
		}

		//todo change each links from and to entities (empty entitites - ports)
		for (auto linkId : manager->getGroup<LinkEntity>(manager->grid->getGridLevel() == Grid::Level::Basic ? Manager::Manager::groupLinks_0 :
			(manager->grid->getGridLevel() == Grid::Level::Outer1 ? Manager::Manager::groupGroupLinks_0 :
				Manager::groupGroupLinks_1
				)))
		{
			auto* link = dynamic_cast<LinkEntity*>(manager->getEntityFromId(linkId));

			link->updateConnectedPorts();
			link->addArrowHead();
		}
	}

	if ((manager->last_arrowheadsEnabled != manager->arrowheadsEnabled) || manager->updateInnerPathLinks) {
		manager->last_arrowheadsEnabled = manager->arrowheadsEnabled;

		if (manager->arrowheadsEnabled) {

			for (auto linkId : manager->getGroup<LinkEntity>(Manager::groupPathLinks))
			{
				auto* link = dynamic_cast<LinkEntity*>(manager->getEntityFromId(linkId));

				link->updateConnectedPorts();
			}

		}
		if (!manager->arrowheadsEnabled) {

			//todo change each links from and to entities (from ports, to center of nodes)
			for (auto linkId : manager->getGroup<LinkEntity>(Manager::groupLinks_0)) {
				auto* link = dynamic_cast<LinkEntity*>(manager->getEntityFromId(linkId));

				link->resetPorts();
				link->removeArrowHead();
			}
			for (auto linkId : manager->getGroup<LinkEntity>(Manager::groupGroupLinks_0)) {
				auto* link = dynamic_cast<LinkEntity*>(manager->getEntityFromId(linkId));
				link->resetPorts();
				link->removeArrowHead();
			}
			for (auto linkId : manager->getGroup<LinkEntity>(Manager::groupGroupLinks_1)) {
				auto* link = dynamic_cast<LinkEntity*>(manager->getEntityFromId(linkId));
				link->resetPorts();
				link->removeArrowHead();
			}

			for (auto linkId : manager->getGroup<LinkEntity>(Manager::groupPathLinks)) {
				auto* link = dynamic_cast<LinkEntity*>(manager->getEntityFromId(linkId));
				link->resetPorts();
				link->removeArrowHead();
			}
			for (auto linkId : manager->getGroup<LinkEntity>(Manager::groupPathInnerLinks)) {
				auto* link = dynamic_cast<LinkEntity*>(manager->getEntityFromId(linkId));
				link->resetPorts();
				link->removeArrowHead();
			}
			//todo remove all ports
			for (auto nodeId : manager->getGroup<NodeEntity>(Manager::groupNodes_0)) {
				auto* node = dynamic_cast<NodeEntity*>(manager->getEntityFromId(nodeId));
				node->removePorts();
			}
			for (auto nodeId : manager->getGroup<NodeEntity>(Manager::groupGroupNodes_0)) {
				auto* node = dynamic_cast<NodeEntity*>(manager->getEntityFromId(nodeId));
				node->removePorts();
			}
			for (auto nodeId : manager->getGroup<NodeEntity>(Manager::groupGroupNodes_1)) {
				auto* node = dynamic_cast<NodeEntity*>(manager->getEntityFromId(nodeId));
				node->removePorts();
			}
		}

		if (manager->updateInnerPathLinks) {
			for (auto pathLinkerId : manager->getGroup<EmptyEntity>(Manager::groupPathLinksHolder))
			{
				auto* pathLinker = dynamic_cast<EmptyEntity*>(manager->getEntityFromId(pathLinkerId));

				pathLinker->GetComponent<PathLinkerComponent>().removeInnerLinks();
			}
			if (manager->arrowheadsEnabled) {
				for (auto pathLinkerId : manager->getGroup<EmptyEntity>(Manager::groupPathLinksHolder))
				{
					auto* pathLinker = dynamic_cast<EmptyEntity*>(manager->getEntityFromId(pathLinkerId));
					pathLinker->GetComponent<PathLinkerComponent>().createInnerLinks();
				}
			}
			manager->updateInnerPathLinks = false;
		}

		manager->aboutTo_updateActiveEntities();

	}


	// check input manager if left mouse is clicked, if yes and the mouse is not on the widget then nullify displayedEntity
	if (_app->_inputManager.isKeyPressed(SDL_BUTTON_LEFT))
	{
		if (!ImGuiInterface::isMouseOnWidget("Node Display") &&
			!ImGuiInterface::isMouseOnWidget("Link Display") &&
			!ImGuiInterface::isMouseOnWidget("Empty Display") &&
			!ImGuiInterface::isMouseOnWidget("Scene Manager")) {
			_displayedEntity = nullptr;
			_sceneManagerActive = false;
		}
	}

	if (_app->_inputManager.isKeyPressed(SDL_BUTTON_RIGHT))
	{
		_graphEditorLayer.getSubcomponent<GraphMiddlePanel>()->
			getSubcomponent<ViewportPanel>()->
			getSubcomponent<SceneControlPanel>()->savedMousePos = getApp()->_inputManager.getMouseCoords();
	}

}
