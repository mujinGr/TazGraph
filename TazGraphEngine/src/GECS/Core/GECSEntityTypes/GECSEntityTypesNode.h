#pragma once

#include "./GECSEntityTypes.h"

class Node : public NodeEntity {
private:

public:

	Node(Manager& mManager) : NodeEntity(mManager) {


	}

	void addGroup(Group mGroup) override {
		Entity::addGroup(mGroup);
		manager.AddToGroup(this, mGroup);
	}

	virtual ~Node() {

	}

	void update(float deltaTime)
	{
		//cellUpdate();

		Entity::update(deltaTime);
	}

	void cellUpdate() override {
		if (this->ownerCell) {
			//this->GetComponent<TransformComponent>().update(0.0f);
			Cell* newCell = manager.grid->getCell(*this, manager.grid->getGridLevel());
			if (newCell != this->ownerCell) {
				removeFromCell();
				manager.grid->addNode(this, newCell);
			}

			{
				std::scoped_lock lock(manager.movedNodesMutex);

				manager.movedNodes.push_back(getId());
			}

			// update arrowheads
			for (auto& linkId : inLinks) {
				auto* link = dynamic_cast<LinkEntity*>(manager.getEntityFromId(linkId));
				if (link->type == LinkEntity::ConnectionType::PORT_TO_PORT)
					link->updateArrowHeads();

				for (auto& depthLinkId : dynamic_cast<NodeEntity*>(manager.getEntityFromId(link->getFromNode()))->getInLinks()) {
					auto* depthLink = dynamic_cast<LinkEntity*>(manager.getEntityFromId(depthLinkId));
					if (depthLink->type == LinkEntity::ConnectionType::PORT_TO_PORT)
						depthLink->updateArrowHeads();
				}
				for (auto& depthLinkId : dynamic_cast<NodeEntity*>(manager.getEntityFromId(link->getFromNode()))->getOutLinks()) {
					auto* depthLink = dynamic_cast<LinkEntity*>(manager.getEntityFromId(depthLinkId));
					if (depthLink->type == LinkEntity::ConnectionType::PORT_TO_PORT)
						depthLink->updateArrowHeads();
				}
			}
			for (auto& linkId : outLinks) {
				auto* link = dynamic_cast<LinkEntity*>(manager.getEntityFromId(linkId));
				if (link->type == LinkEntity::ConnectionType::PORT_TO_PORT)
					link->updateArrowHeads();
				for (auto& depthLinkId : dynamic_cast<NodeEntity*>(manager.getEntityFromId(link->getToNode()))->getInLinks()) {
					auto* depthLink = dynamic_cast<LinkEntity*>(manager.getEntityFromId(depthLinkId));
					if (depthLink->type == LinkEntity::ConnectionType::PORT_TO_PORT)
						depthLink->updateArrowHeads();
				}
				for (auto& depthLinkId : dynamic_cast<NodeEntity*>(manager.getEntityFromId(link->getToNode()))->getOutLinks()) {
					auto* depthLink = dynamic_cast<LinkEntity*>(manager.getEntityFromId(depthLinkId));
					if (depthLink->type == LinkEntity::ConnectionType::PORT_TO_PORT)
						depthLink->updateArrowHeads();
				}
			}
		}
	}

	void imgui_print() override {
		glm::vec2 position = this->GetComponent<TransformComponent>().getPosition();  // Make sure Entity class has a getPosition method
		ImGui::Text("TazPosition: (%.2f, %.2f)", position.x, position.y);


	}

	void imgui_display() override {
		ImGui::Text("Display Info Here Node");
	}

	void destroy() {
		Entity::destroy();
		manager.aboutTo_updateActiveEntities();//? cant have it at destroy in baseclass
	}

	void addPorts() {
		auto createPort = [this](NodePorts portName, const glm::vec3& localPosition, bool isHorizontal) {
			const char* t_portName = NodePorts_ToString(portName);

			if (children.contains(t_portName)) {
				TazGraphEngine::ConsoleLogger::error(
					std::string("Port already exists: ") + t_portName);
				return;
			}

			auto& port = getManager()->addEntityFromParent<Empty>(this, t_portName);
			port.addGroup(Manager::groupPorts);
			port.addComponent<TransformComponent>(glm::vec3(0), glm::vec3(0), 1.0f);


			children[t_portName] = port.getId();
			getManager()->getEntityFromId(children[t_portName])->setParentEntity(this);
			getManager()->getEntityFromId(children[t_portName])->GetComponent<TransformComponent>().local_normal_position = localPosition;
			getManager()->getEntityFromId(children[t_portName])->GetComponent<TransformComponent>().initChild();
			getManager()->getEntityFromId(children[t_portName])->addComponent<PortComponent>(isHorizontal);
			getManager()->getEntityFromId(children[t_portName])->update(0.0f);
			};

		// Create all ports using the lambda
		createPort(NodePorts::LEFT, glm::vec3(-1.0f, 0.0f, 0.0f), true);
		createPort(NodePorts::RIGHT, glm::vec3(1.0f, 0.0f, 0.0f), true);
		createPort(NodePorts::TOP, glm::vec3(0.0f, -1.0f, 0.0f), false);
		createPort(NodePorts::BOTTOM, glm::vec3(0.0f, 1.0f, 0.0f), false);

	}

	void removePorts() override {
		for (auto portName : { NodePorts::LEFT, NodePorts::RIGHT, NodePorts::TOP, NodePorts::BOTTOM }) {

			const char* t_portName = NodePorts_ToString(portName);

			if (children.contains(t_portName)) {
				getManager()->getEntityFromId(children[t_portName])->destroy();
				for (auto& slot : getManager()->getEntityFromId(children[t_portName])->children)
				{
					auto* slotEnt = getManager()->getEntityFromId(slot.second);
					slotEnt->destroy();
				}
				getManager()->getEntityFromId(children[t_portName])->children.clear();
				children.erase(t_portName);
			}
		}
	}

	void removeSlots() override {
		for (auto portName : { NodePorts::LEFT, NodePorts::RIGHT, NodePorts::TOP, NodePorts::BOTTOM }) {
			const char* t_portName = NodePorts_ToString(portName);

			if (children.contains(t_portName)) {
				for (auto slot : getManager()->getEntityFromId(children[t_portName])->children)
				{
					getManager()->getEntityFromId(slot.second)->destroy();
				}
				getManager()->getEntityFromId(children[t_portName])->children.clear();
			}
		}
	}

};


