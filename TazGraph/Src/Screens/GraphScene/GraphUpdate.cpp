#include "Graph.h"
#include <AppScene/AppInterface.h>

void Graph::update(float deltaTime) //game objects updating
{
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));
	std::shared_ptr<OrthoCamera> minimap_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("minimap"));

	glm::vec2 mouseCoordsVec = _sceneMousePosition;

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

	for (auto& node : manager->getVisibleGroup<NodeEntity>(Manager::groupColliders)) {
		node->GetComponent<ColliderComponent>().collisionPhysics();
	}

	/*
	
	action: add path links

	-> if arrowheads are enabled, 


	
	*/

	if ((manager->last_arrowheadsEnabled != manager->arrowheadsEnabled)
		&& manager->arrowheadsEnabled) {
		//todo add to all nodes ports
		for (auto& node : manager->getGroup<NodeEntity>(manager->grid->getGridLevel() == Grid::Level::Basic ? Manager::groupNodes_0 :
			(manager->grid->getGridLevel() == Grid::Level::Outer1 ? Manager::groupGroupNodes_0 :
				Manager::groupGroupNodes_1
				)))
		{
			node->addPorts();
		}

		//todo change each links from and to entities (empty entitites - ports)
		for (auto& link : manager->getGroup<LinkEntity>(manager->grid->getGridLevel() == Grid::Level::Basic ? Manager::Manager::groupLinks_0 :
			(manager->grid->getGridLevel() == Grid::Level::Outer1 ? Manager::Manager::groupGroupLinks_0 :
				Manager::groupGroupLinks_1
				)))
		{
			link->updateConnectedPorts();
			link->addArrowHead();
		}
	}

	if ((manager->last_arrowheadsEnabled != manager->arrowheadsEnabled) || manager->updateInnerPathLinks) {
		manager->last_arrowheadsEnabled = manager->arrowheadsEnabled;

		if (manager->arrowheadsEnabled) {

			for (auto& link : manager->getGroup<LinkEntity>(Manager::groupPathLinks))
			{
				link->updateConnectedPorts();
			}

		}
		if (!manager->arrowheadsEnabled) {

			//todo change each links from and to entities (from ports, to center of nodes)
			for (auto& link : manager->getGroup<LinkEntity>(Manager::groupLinks_0)) {
				link->resetPorts();
				link->removeArrowHead();
			}
			for (auto& link : manager->getGroup<LinkEntity>(Manager::groupGroupLinks_0)) {
				link->resetPorts();
				link->removeArrowHead();
			}
			for (auto& link : manager->getGroup<LinkEntity>(Manager::groupGroupLinks_1)) {
				link->resetPorts();
				link->removeArrowHead();
			}

			for (auto& link : manager->getGroup<LinkEntity>(Manager::groupPathLinks)) {
				link->resetPorts();
				link->removeArrowHead();
			}
			for (auto& link : manager->getGroup<LinkEntity>(Manager::groupPathInnerLinks)) {
				link->resetPorts();
				link->removeArrowHead();
			}
			//todo remove all ports
			for (auto& node : manager->getGroup<NodeEntity>(Manager::groupNodes_0)) {
				node->removePorts();
			}
			for (auto& node : manager->getGroup<NodeEntity>(Manager::groupGroupNodes_0)) {
				node->removePorts();
			}
			for (auto& node : manager->getGroup<NodeEntity>(Manager::groupGroupNodes_1)) {
				node->removePorts();
			}
		}

		if(manager->updateInnerPathLinks) {
			for (auto& pathLinker : manager->getGroup<EmptyEntity>(Manager::groupPathLinksHolder))
			{
				pathLinker->GetComponent<PathLinkerComponent>().removeInnerLinks();
			}
			if (manager->arrowheadsEnabled) {
				for (auto& pathLinker : manager->getGroup<EmptyEntity>(Manager::groupPathLinksHolder))
				{
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

}
