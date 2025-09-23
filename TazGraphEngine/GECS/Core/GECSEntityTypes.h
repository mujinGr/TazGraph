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
		ImGui::Text("Position: (%.2f, %.2f)", position.x, position.y);
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

		if (children[NodePorts::LEFT]) {
			for (auto& portSlots : children[NodePorts::LEFT]->GetComponent<PortComponent>().portSlots) {
				portSlots->update(deltaTime);
			}
		}

		if (children[NodePorts::RIGHT]) {
			for (auto& portSlots : children[NodePorts::RIGHT]->GetComponent<PortComponent>().portSlots) {
				portSlots->update(deltaTime);
			}
		}

		if (children[NodePorts::TOP]) {
			for (auto& portSlots : children[NodePorts::TOP]->GetComponent<PortComponent>().portSlots) {
				portSlots->update(deltaTime);
			}
		}

		if (children[NodePorts::BOTTOM]) {
			for (auto& portSlots : children[NodePorts::BOTTOM]->GetComponent<PortComponent>().portSlots) {
				portSlots->update(deltaTime);
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
		ImGui::Text("Position: (%.2f, %.2f)", position.x, position.y);


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
		bool hasChildren = std::all_of(children.begin(), children.end(),
			[](auto& pair) { return pair.second != nullptr; });

		if (hasChildren) {
			TazGraphEngine::ConsoleLogger::error("Node already has 4 children!");
			return;
		}

		TransformComponent* tr = &GetComponent<TransformComponent>();

		auto& leftPort = getManager()->addEntityNoId<Empty>();
		leftPort.addGroup(Manager::groupPorts);
		glm::vec3 m_position = glm::vec3(-tr->size.x / 2, 0.0f, 0.0f);
		leftPort.addComponent<TransformComponent>(m_position, glm::vec3(0), 1.0f);
		children[NodePorts::LEFT] = &leftPort;
		children[NodePorts::LEFT]->setParentEntity(this);
		children[NodePorts::LEFT]->GetComponent<TransformComponent>().local_position = m_position;
		children[NodePorts::LEFT]->GetComponent<TransformComponent>().initChild(0.0f);
		children[NodePorts::LEFT]->addComponent<PortComponent>(true);
		children[NodePorts::LEFT]->update(0.0f);

		auto& rightPort = getManager()->addEntityNoId<Empty>();
		rightPort.addGroup(Manager::groupPorts);
		m_position = glm::vec3(tr->size.x / 2, 0.0f, 0.0f);
		rightPort.addComponent<TransformComponent>(m_position, glm::vec3(0), 1.0f);
		children[NodePorts::RIGHT] = &rightPort;
		children[NodePorts::RIGHT]->setParentEntity(this);
		children[NodePorts::RIGHT]->GetComponent<TransformComponent>().local_position = m_position;
		children[NodePorts::RIGHT]->GetComponent<TransformComponent>().initChild(0.0f);
		children[NodePorts::RIGHT]->addComponent<PortComponent>(true);
		children[NodePorts::RIGHT]->update(0.0f);

		// Initialize top port
		auto& topPort = getManager()->addEntityNoId<Empty>();
		topPort.addGroup(Manager::groupPorts);
		m_position = glm::vec3(0.0f, -tr->size.y / 2.0f, 0.0f);
		topPort.addComponent<TransformComponent>(m_position, glm::vec3(0), 1.0f);
		children[NodePorts::TOP] = &topPort;
		children[NodePorts::TOP]->setParentEntity(this);
		children[NodePorts::TOP]->GetComponent<TransformComponent>().local_position = m_position;
		children[NodePorts::TOP]->GetComponent<TransformComponent>().initChild(0.0f);
		children[NodePorts::TOP]->addComponent<PortComponent>(false);
		children[NodePorts::TOP]->update(0.0f);

		// Initialize bottom port
		auto& bottomPort = getManager()->addEntityNoId<Empty>();
		bottomPort.addGroup(Manager::groupPorts);
		m_position = glm::vec3(0.0f, tr->size.y / 2.0f, 0.0f);
		bottomPort.addComponent<TransformComponent>(m_position, glm::vec3(0), 1.0f);
		children[NodePorts::BOTTOM] = &bottomPort;
		children[NodePorts::BOTTOM]->setParentEntity(this);
		children[NodePorts::BOTTOM]->GetComponent<TransformComponent>().local_position = m_position;
		children[NodePorts::BOTTOM]->GetComponent<TransformComponent>().initChild(0.0f);
		children[NodePorts::BOTTOM]->addComponent<PortComponent>(false);
		children[NodePorts::BOTTOM]->update(0.0f);

		/*	auto& testSlot = getManager()->addEntityNoId<Empty>();
			testSlot.addGroup(Manager::groupPortSlots);
			testSlot.addComponent<TransformComponent>(m_position, glm::vec3(3), 1.0f);
			testSlot.addComponent<Rectangle_w_Color>();
			testSlot.GetComponent<Rectangle_w_Color>().setColor(Color(0, 250, 0, 255));
			testSlot.addComponent<PortSlotComponent>();
			testSlot.setParentEntity(children[NodePorts::BOTTOM]);
			children[NodePorts::BOTTOM]->GetComponent<PortComponent>().
				portSlots.push_back(&testSlot);*/

	}

	void removePorts() override {
		for (auto portName : { NodePorts::LEFT, NodePorts::RIGHT, NodePorts::TOP, NodePorts::BOTTOM }) {
			if (children[portName]) {
				children[portName]->destroy();
				for (auto slot : children[portName]->GetComponent<PortComponent>().portSlots)
				{
					slot->destroy();
				}
				children[portName]->GetComponent<PortComponent>().portSlots.clear();
				children[portName] = nullptr;
			}
		}
	}

	void removeSlots() override {
		for (auto portName : { NodePorts::LEFT, NodePorts::RIGHT, NodePorts::TOP, NodePorts::BOTTOM }) {
			if (children[portName]) {
				for (auto slot : children[portName]->GetComponent<PortComponent>().portSlots)
				{
					slot->destroy();
				}
				children[portName]->GetComponent<PortComponent>().portSlots.clear();
			}
		}
	}

};


class Link : public LinkEntity {
private:



public:


	Color color = {};

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
		if (children[LinkPorts::ARROWHEAD]) {
			TransformComponent* tr = &children[LinkPorts::ARROWHEAD]->GetComponent<TransformComponent>();

			children[LinkPorts::ARROWHEAD]->update(0.0f);

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
			children[LinkPorts::ARROWHEAD]->GetComponent<TransformComponent>().position = newArrowHeadPosition;

			children[LinkPorts::ARROWHEAD]->GetComponent<TransformComponent>().setRotation(glm::vec3(0.0f, 0.0f, angleRadians + glm::half_pi<float>()));
		}
	}

	void updateConnectedPorts() override {
		TransformComponent* toTR = &to->GetComponent<TransformComponent>();
		TransformComponent* fromTR = &from->GetComponent<TransformComponent>();

		int newFromPort = getBestPortForConnection(fromTR->getPosition(), toTR->getPosition());
		int newToPort = getBestPortForConnection(toTR->getPosition(), fromTR->getPosition());

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
		temp_arrowHead.GetComponent<Triangle_w_Color>().color = Color(0, 0, 0, 255);

		temp_arrowHead.GetComponent<TransformComponent>().setRotation(glm::vec3(0.0f, 0.0f, angleRadians + glm::half_pi<float>()));

		temp_arrowHead.addGroup(Manager::groupArrowHeads_0);

		temp_arrowHead.setParentEntity(this);

		manager.grid->addEmpty(&temp_arrowHead, manager.grid->getGridLevel());

		children[LinkPorts::ARROWHEAD] = &temp_arrowHead;
	}

	void removeArrowHead() override {
		if (children[LinkPorts::ARROWHEAD]) {
			children[LinkPorts::ARROWHEAD]->removeFromCell();
			children[LinkPorts::ARROWHEAD]->destroy();
			children[LinkPorts::ARROWHEAD] = nullptr;
		}
	}

	void resetPorts() override {
		fromPort = -1;
		toPort = -1;
	}

	int getBestPortForConnection(const glm::vec3& fromPos, const glm::vec3& toPos) {
		// Simple logic to determine the port based on relative position
		float deltaX = toPos.x - fromPos.x;
		float deltaY = toPos.y - fromPos.y;

		if (abs(deltaX) > abs(deltaY)) {  // Horizontal distance is greater
			return deltaX > 0 ? NodePorts::RIGHT : NodePorts::LEFT;
		}
		else {  // Vertical distance is greater
			return deltaY > 0 ? NodePorts::BOTTOM : NodePorts::TOP;
		}
	}

	int assignSlotIndex(NodeEntity* node, EntityID newPort, EntityID oldPort, int oldSlotIndex) {
		// If port changed, remove link from old port's slots
		if (std::get<int>(oldPort) != -1 && oldPort != newPort && std::get<int>(oldPort) < (int)node->children.size()) {
			Entity* oldPortEntity = node->children[oldPort];
			if (oldPortEntity && oldPortEntity->hasComponent<PortComponent>()) {
				auto& oldSlots = oldPortEntity->GetComponent<PortComponent>().portSlots;

				if (oldSlotIndex >= 0 && oldSlotIndex < (int)oldSlots.size()) {
					EmptyEntity* slotToRemove = oldSlots[oldSlotIndex];

					oldSlots.erase(oldSlots.begin() + oldSlotIndex);  // Remove from vector first
					slotToRemove->destroy();

					updateLinksSlotIndices(node, oldPort, oldSlotIndex);
				}
			}
		}

		if (oldPort != newPort) {
			if (std::get<int>(newPort) < 0 || std::get<int>(newPort) >= (int)node->children.size()) {
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
			newSlot.GetComponent<Rectangle_w_Color>().setColor(Color(0, 250, 0, 255)); // Green for connected
			newSlot.addComponent<PortSlotComponent>();
			// Store reference to the link (you might want to add this field to PortSlotComponent)
			// newSlot.GetComponent<PortSlotComponent>().linkedEntity = link;
			newSlot.setParentEntity(newPortEntity);
			newSlots.push_back(&newSlot);

			return static_cast<int>(newSlots.size() - 1);
		}

		return oldSlotIndex;
	}

	void updateLinksSlotIndices(NodeEntity* node, EntityID portIndex, int removedSlotIndex) {
		for (auto& linkEntity : node->getOutLinks()) {

			if (linkEntity->fromPort == portIndex &&
				linkEntity->fromSlotIndex > removedSlotIndex) {
				linkEntity->fromSlotIndex--;
			}
		}

		for (auto& linkEntity : node->getInLinks()) {

			if (linkEntity->toPort == portIndex &&
				linkEntity->toSlotIndex > removedSlotIndex) {
				linkEntity->toSlotIndex--;
			}
		}
	}

	void imgui_print() override {
		glm::vec2 fromNodePosition = this->getFromNode()->GetComponent<TransformComponent>().getPosition();
		glm::vec2 toNodePosition = this->getToNode()->GetComponent<TransformComponent>().getPosition();

		ImGui::Text("From Node Position: (%.2f, %.2f)", fromNodePosition.x, fromNodePosition.y);
		ImGui::Text("To Node Position: (%.2f, %.2f)", toNodePosition.x, toNodePosition.y);

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

		if (children[LinkPorts::ARROWHEAD]) {
			children[LinkPorts::ARROWHEAD]->destroy();
		}

		manager.aboutTo_updateActiveEntities();
	}

};