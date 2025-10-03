#pragma once

#include "GECSManager.h"
#include "../Components.h"


class Empty : public EmptyEntity {

public:

	Empty(Manager& mManager) : EmptyEntity(mManager) {

	}

	void addGroup(Group mGroup) override {
		Entity::addGroup(mGroup);
		manager.AddToGroup(this, mGroup);
	}

	virtual ~Empty() {

	}

	void update(float deltaTime)
	{
		//cellUpdate();

		Entity::update(deltaTime);
	}

	void cellUpdate() override {
		if (this->ownerCell) {
			Cell* newCell = manager.grid->getCell(*this, manager.grid->getGridLevel());
			if (newCell != this->ownerCell) {
				// Need to shift the entity
				removeEntity();
				manager.grid->addEmpty(this, newCell);
			}
		}
	}



	void imgui_print() override {
		glm::vec2 position = this->GetComponent<TransformComponent>().getPosition();  // Make sure Entity class has a getPosition method
		ImGui::Text("TazPosition: (%.2f, %.2f)", position.x, position.y);
	}

	void destroy() {
		Entity::destroy();
		manager.aboutTo_updateActiveEntities(); // cant have it at destroy in baseclass
		// may need to also update Visible Entities
	}
};

class Node : public NodeEntity {
private:


	std::vector<std::string> messageLog;
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

	void updatePorts(float deltaTime) override {
		// first set the new position of port based on parent size
		// then set its bodyPosition through the transformComponent

		TransformComponent* tr = &GetComponent<TransformComponent>();

		for (auto portName : { LEFT, RIGHT, TOP, BOTTOM }) {
			auto it = children.find(NodePorts_ToString(portName));
			if (it != children.end() && it->second) {
				for (auto& portSlots : it->second->GetComponent<PortComponent>().portSlots) {
					portSlots->update(deltaTime);
				}
			}
		}
	}

	void cellUpdate() override {
		if (this->ownerCell) {
			updatePorts(0.0f);
			//this->GetComponent<TransformComponent>().update(0.0f);
			Cell* newCell = manager.grid->getCell(*this, manager.grid->getGridLevel());
			if (newCell != this->ownerCell) {
				std::scoped_lock lock(manager.movedNodesMutex);
				removeEntity();
				manager.grid->addNode(this, newCell);

				manager.movedNodes.push_back(this);
			}
			for (auto& link : inLinks) {
				link->updateArrowHeads();
			}
			for (auto& link : outLinks) {
				link->updateArrowHeads();
			}
		}
	}

	void addMessage(std::string mMessage) override {
		messageLog.push_back(mMessage);
	}

	void imgui_print() override {
		glm::vec2 position = this->GetComponent<TransformComponent>().getPosition();  // Make sure Entity class has a getPosition method
		ImGui::Text("TazPosition: (%.2f, %.2f)", position.x, position.y);


		if (ImGui::BeginTable("GroupsTable", 1, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
			ImGui::TableSetupColumn("Message Log", ImGuiTableColumnFlags_WidthStretch);

			ImGui::TableHeadersRow();
			for (auto str : messageLog) {
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s", str.c_str());
			}
		}
		ImGui::EndTable();


	}

	void imgui_display() override {
		ImGui::Text("Display Info Here Node");
	}

	void destroy() {
		Entity::destroy();
		manager.aboutTo_updateActiveEntities();
	}

	void addPorts() {
		auto createPort = [this](NodePorts portName, const glm::vec3& localPosition, bool isHorizontal) {
			const char* t_portName = NodePorts_ToString(portName);

			if (children.contains(t_portName)) {
				TazGraphEngine::ConsoleLogger::error(
					std::string("Port already exists: ") + t_portName);
				return;
			}

			auto& port = getManager()->addEntityNoId<Empty>();
			port.addGroup(Manager::groupPorts);
			port.addComponent<TransformComponent>(glm::vec3(0), glm::vec3(0), 1.0f);


			children[t_portName] = &port;
			children[t_portName]->setParentEntity(this, t_portName);
			children[t_portName]->GetComponent<TransformComponent>().local_normal_position = localPosition;
			children[t_portName]->GetComponent<TransformComponent>().initChild();
			children[t_portName]->addComponent<PortComponent>(isHorizontal);
			children[t_portName]->update(0.0f);
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
				children[t_portName]->destroy();
				for (auto slot : children[t_portName]->GetComponent<PortComponent>().portSlots)
				{
					slot->destroy();
				}
				children[t_portName]->GetComponent<PortComponent>().portSlots.clear();
				children.erase(t_portName);
			}
		}
	}

	void removeSlots() override {
		for (auto portName : { NodePorts::LEFT, NodePorts::RIGHT, NodePorts::TOP, NodePorts::BOTTOM }) {
			const char* t_portName = NodePorts_ToString(portName);

			if (children.contains(t_portName)) {
				for (auto slot : children[t_portName]->GetComponent<PortComponent>().portSlots)
				{
					slot->destroy();
				}
				children[t_portName]->GetComponent<PortComponent>().portSlots.clear();
			}
		}
	}

};


class Link : public LinkEntity {
private:



public:


	TazColor color = {};

	Link(Manager& mManager) : LinkEntity(mManager) {
	}

	Link(Manager& mManager, EntityID mfromId, EntityID mtoId)
		: LinkEntity(mManager, mfromId, mtoId)
	{
		from = dynamic_cast<NodeEntity*>(mManager.getEntityFromId(fromId));
		from->addOutLink(this);
		to = dynamic_cast<NodeEntity*>(mManager.getEntityFromId(toId));
		to->addInLink(this);
	}

	Link(Manager& mManager, Entity* mfrom, Entity* mto)
		: LinkEntity(mManager,
			dynamic_cast<NodeEntity*>(mfrom), // it is node but cant see it due to getParentEntity
			dynamic_cast<NodeEntity*>(mto))
	{
		fromId = from->getId();
		toId = to->getId();
	}

	Link(Manager& mManager, NodeEntity* mfrom, NodeEntity* mto)
		: LinkEntity(mManager,
			mfrom,
			mto)
	{
		fromId = from->getId();
		toId = to->getId();
	}

	Link(Manager& mManager,
		EntityID mfromId, EntityID mtoId,
		NodeEntity* mfrom, NodeEntity* mto
	)
		: LinkEntity(mManager,
			mfromId, mtoId,
			mfrom, mto)
	{

	}

	Link(
		Manager& mManager,
		NodeEntity* mfrom, NodeEntity* mto,
		EntityID m_fromPort, EntityID m_toPort, int m_fromSlot, int m_toSlot
	)
		: LinkEntity(mManager,
			mfrom, mto,
			m_fromPort, m_toPort,
			m_fromSlot, m_toSlot)
	{

	}

	void addGroup(Group mGroup) override {
		Entity::addGroup(mGroup);
		manager.AddLinkToGroup(this, mGroup);
	}

	virtual ~Link() {

	}

	void update(float deltaTime) override
	{
		Entity::update(deltaTime);


	}

	void cellUpdate() override {
		// if cell(or position) of fromNode or cell(or position) of toNode is different than
		// the saved cells in ownerCells then update it
		if (!ownerCells.empty()) {
			auto level = manager.grid->getGridLevel();
			const auto& fromCell = manager.grid->getCell(*getFromNode(), level);
			const auto& toCell = manager.grid->getCell(*getToNode(), level);

			const auto& ownerFront = ownerCells.front();
			const auto& ownerBack = ownerCells.back();

			if (fromCell != ownerFront
				|| toCell != ownerBack)
			{
				removeFromCells();

				manager.grid->addLink(this, manager.grid->getGridLevel());
			}
		}

	}

	void updateArrowHeads() override {
		if (children.contains(LinkChildren_ToString(ARROWHEAD))) {
			TransformComponent* tr = &children[LinkChildren_ToString(ARROWHEAD)]->GetComponent<TransformComponent>();

			children[LinkChildren_ToString(ARROWHEAD)]->update(0.0f);

			// set position of arrowHead
			TransformComponent* ch_tr = &to->children[toPort]->GetComponent<TransformComponent>();

			TransformComponent* toPortTR = ch_tr;
			TransformComponent* fromPortTR = &from->children[fromPort]->GetComponent<TransformComponent>();

			glm::vec3 direction = toPortTR->getPosition() - fromPortTR->getPosition();

			glm::vec3 unitDirection = glm::normalize(direction);
			float offset = 10.0f;

			glm::vec3 arrowHeadPos = toPortTR->getPosition() - unitDirection * offset;

			// Calculate the angle in radians, and convert it to degrees
			float angleRadians = -atan2(direction.y, direction.x);
			float angleDegrees = glm::degrees(angleRadians);


			glm::vec3 newArrowHeadPosition = arrowHeadPos;
			children[LinkChildren_ToString(ARROWHEAD)]->GetComponent<TransformComponent>().position = newArrowHeadPosition;

			children[LinkChildren_ToString(ARROWHEAD)]->GetComponent<TransformComponent>().setRotation(glm::vec3(0.0f, 0.0f, angleRadians + glm::half_pi<float>()));
		}
	}

	void updateConnectedPorts() override {
		TransformComponent* toTR = &to->GetComponent<TransformComponent>();
		TransformComponent* fromTR = &from->GetComponent<TransformComponent>();

		EntityID newFromPort = getBestPortForConnection(fromTR->getPosition(), toTR->getPosition());
		EntityID newToPort = getBestPortForConnection(toTR->getPosition(), fromTR->getPosition());

		fromSlotIndex = assignSlotIndex(from, newFromPort, fromPort, fromSlotIndex);
		toSlotIndex = assignSlotIndex(to, newToPort, toPort, toSlotIndex);

		fromPort = newFromPort;
		toPort = newToPort;
	}

	void addArrowHead() override {
		TransformComponent* toTR = &to->GetComponent<TransformComponent>();
		TransformComponent* fromTR = &from->GetComponent<TransformComponent>();

		fromPort = getBestPortForConnection(fromTR->getPosition(), toTR->getPosition());
		toPort = getBestPortForConnection(toTR->getPosition(), fromTR->getPosition());

		TransformComponent* toPortTR = &to->children[toPort]->GetComponent<TransformComponent>();
		TransformComponent* fromPortTR = &from->children[fromPort]->GetComponent<TransformComponent>();

		glm::vec3 direction = toPortTR->getPosition() - fromPortTR->getPosition();

		glm::vec3 unitDirection = glm::normalize(direction);
		float offset = 10.0f;

		glm::vec3 arrowHeadPos = toPortTR->getPosition() - unitDirection * offset;

		auto& temp_arrowHead = getManager()->addEntityNoId<Empty>();

		// Calculate the angle in radians, and convert it to degrees
		float angleRadians = -atan2(direction.y, direction.x);
		float angleDegrees = glm::degrees(angleRadians);

		glm::vec3 farrowSize(10.0f, 20.0f, 0.0f);

		temp_arrowHead.addComponent<TransformComponent>(arrowHeadPos, farrowSize, 1);
		temp_arrowHead.addComponent<Triangle_w_Color>();
		temp_arrowHead.GetComponent<Triangle_w_Color>().color = TazColor(0, 0, 0, 255);

		temp_arrowHead.GetComponent<TransformComponent>().setRotation(glm::vec3(0.0f, 0.0f, angleRadians + glm::half_pi<float>()));

		temp_arrowHead.addGroup(Manager::groupArrowHeads_0);

		temp_arrowHead.setParentEntity(this, LinkChildren_ToString(ARROWHEAD));

		manager.grid->addEmpty(&temp_arrowHead, manager.grid->getGridLevel());

		children[LinkChildren_ToString(ARROWHEAD)] = &temp_arrowHead;
	}

	void removeArrowHead() override {
		if (children.contains(LinkChildren_ToString(ARROWHEAD))) {
			children[LinkChildren_ToString(ARROWHEAD)]->removeFromCell();
			children[LinkChildren_ToString(ARROWHEAD)]->destroy();
			children[LinkChildren_ToString(ARROWHEAD)] = nullptr;
			children.erase(LinkChildren_ToString(ARROWHEAD));
		}
	}

	void resetPorts() override {
		fromPort = -1;
		toPort = -1;
	}

	EntityID getBestPortForConnection(const glm::vec3& fromPos, const glm::vec3& toPos) {
		// Simple logic to determine the port based on relative position
		float deltaX = toPos.x - fromPos.x;
		float deltaY = toPos.y - fromPos.y;

		if (abs(deltaX) > abs(deltaY)) {  // Horizontal distance is greater
			return deltaX > 0 ? NodePorts_ToString(RIGHT) : NodePorts_ToString(LEFT);
		}
		else {  // Vertical distance is greater
			return deltaY > 0 ? NodePorts_ToString(BOTTOM) : NodePorts_ToString(TOP);
		}
	}

	int assignSlotIndex(NodeEntity* node, EntityID newPort, EntityID oldPort, int oldSlotIndex) {
		// If port changed, remove link from old port's slots
		if (std::holds_alternative<std::string>(oldPort) &&
			!std::get<std::string>(oldPort).empty() &&
			oldPort != newPort) {
			Entity* oldPortEntity = node->children[oldPort];
			if (oldPortEntity && oldPortEntity->hasComponent<PortComponent>()) {
				auto& oldSlots = oldPortEntity->GetComponent<PortComponent>().portSlots;

				if (oldSlotIndex >= 0 && oldSlotIndex < (int)oldSlots.size()) {
					EmptyEntity* slotToRemove = oldSlots[oldSlotIndex];

					oldSlots.erase(oldSlots.begin() + oldSlotIndex);  // Remove from vector first
					slotToRemove->destroy();

					updateLinksSlotIndices(node, oldPort, oldSlotIndex, true);
					updateLinksSlotIndices(node, oldPort, oldSlotIndex, false);
				}
			}
		}

		if (oldPort != newPort) {
			if (std::get<std::string>(newPort).empty()) {
				return -1; // Invalid port
			}

			Entity* newPortEntity = node->children[newPort];
			if (!newPortEntity || !newPortEntity->hasComponent<PortComponent>()) {
				return -1; // Port doesn't exist or doesn't have PortComponent
			}

			auto& newSlots = newPortEntity->GetComponent<PortComponent>().portSlots;

			auto& newSlot = node->getManager()->addEntityNoId<Empty>();
			newSlot.addGroup(Manager::groupPortSlots);
			TransformComponent& portTransform = newPortEntity->GetComponent<TransformComponent>();
			newSlot.addComponent<TransformComponent>(
				portTransform.position,
				glm::vec3(3),
				1.0f
			);
			newSlot.addComponent<Rectangle_w_Color>();
			newSlot.GetComponent<Rectangle_w_Color>().setColor(TazColor(0, 250, 0, 255)); // Green for connected
			newSlot.addComponent<PortSlotComponent>();
			// Store reference to the link (you might want to add this field to PortSlotComponent)
			// newSlot.GetComponent<PortSlotComponent>().linkedEntity = link;
			newSlot.setParentEntity(newPortEntity);
			newSlots.push_back(&newSlot);

			return static_cast<int>(newSlots.size() - 1);
		}

		return oldSlotIndex;
	}

	void updateLinksSlotIndices(NodeEntity* node, EntityID portIndex, int removedSlotIndex, bool isFrom) {

		// reassign slots to links
		if (isFrom) {
			for (auto& linkEntity : node->getOutLinks()) {

				if (linkEntity->fromPort == portIndex &&
					linkEntity->fromSlotIndex > removedSlotIndex) {
					linkEntity->fromSlotIndex--;
				}
			}
		}
		else {
			for (auto& linkEntity : node->getInLinks()) {

				if (linkEntity->toPort == portIndex &&
					linkEntity->toSlotIndex > removedSlotIndex) {
					linkEntity->toSlotIndex--;
				}
			}
		}
	}

	void removeSlotFromNode(NodeEntity* node, EntityID port, int slotIndex, bool isFrom) {
		if (std::holds_alternative<int>(port) || slotIndex == -1) return;

		Entity* portEntity = node->children[port];
		if (portEntity && portEntity->hasComponent<PortComponent>()) {
			auto& portSlots = portEntity->GetComponent<PortComponent>().portSlots;
			if (slotIndex >= 0 && slotIndex < portSlots.size()) {
				portSlots[slotIndex]->destroy();
				portSlots.erase(portSlots.begin() + slotIndex);
				updateLinksSlotIndices(node, port, slotIndex, isFrom);
			}
		}
	}

	void imgui_print() override {
		glm::vec2 fromNodePosition = this->getFromNode()->GetComponent<TransformComponent>().getPosition();
		glm::vec2 toNodePosition = this->getToNode()->GetComponent<TransformComponent>().getPosition();

		ImGui::Text("From Node TazPosition: (%.2f, %.2f)", fromNodePosition.x, fromNodePosition.y);
		ImGui::Text("To Node TazPosition: (%.2f, %.2f)", toNodePosition.x, toNodePosition.y);

		ImGui::Text("Bounding boxes of intercepted cells:");

		for (auto cell : ownerCells) {
			ImGui::Text("- %.2f , %.2f , %.2f", cell->boundingBox_origin.x, cell->boundingBox_origin.y, cell->boundingBox_origin.z);
		}
	}

	void imgui_display() override {
		ImGui::Text("Display Info Here Link");
	}

	void destroy() {
		Entity::destroy();

		NodeEntity* from = getFromNode();
		NodeEntity* to = getToNode();

		if (from) {
			from->removeOutLink(this);
			removeSlotFromNode(from, fromPort, fromSlotIndex, true);
		}
		if (to) {
			to->removeInLink(this);
			removeSlotFromNode(to, toPort, toSlotIndex, false);
		}

		removeArrowHead();
		resetPorts();

		manager.aboutTo_updateActiveEntities();
	}

};