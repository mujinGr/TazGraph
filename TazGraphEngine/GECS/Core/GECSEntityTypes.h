#pragma once

#include "GECSManager.h"
#include "../Components.h"
#include <tracy/public/tracy/Tracy.hpp>

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
		ZoneScoped;

		//cellUpdate();

		Entity::update(deltaTime);
	}

	void cellUpdate() override {
		if (this->ownerCell) {
			Cell* newCell = manager.grid->getCell(*this, manager.grid->getGridLevel());
			if (newCell != this->ownerCell) {
				// Need to shift the entity
				removeFromCell();
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
				std::scoped_lock lock(manager.movedNodesMutex);
				removeFromCell();
				manager.grid->addNode(this, newCell);

				manager.movedNodes.push_back(id);
			}
			for (auto& link : inLinks) {
				link->updateArrowHeads();
				for (auto& depthLink : link->getFromNode()->getInLinks()) {
					depthLink->updateArrowHeads();
				}
				for (auto& depthLink : link->getFromNode()->getOutLinks()) {
					depthLink->updateArrowHeads();
				}
			}
			for (auto& link : outLinks) {
				link->updateArrowHeads();
				for (auto& depthLink : link->getToNode()->getInLinks()) {
					depthLink->updateArrowHeads();
				}
				for (auto& depthLink : link->getToNode()->getOutLinks()) {
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


			children[t_portName] = port.getId();
			getManager()->getEntityFromId(children[t_portName])->setParentEntity(this, t_portName);
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


class Link : public LinkEntity {
private:



public:

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
			NodeEntity* fromNode = getFromNode();
			NodeEntity* toNode = getToNode();
			if (!fromNode || !toNode)
				return;

			Entity* fromPortEntity = nullptr;
			Entity* toPortEntity = nullptr;

			if (fromNode->children.contains(fromPort))
				fromPortEntity = getManager()->getEntityFromId(fromNode->children[fromPort]);
			if (toNode->children.contains(toPort))
				toPortEntity = getManager()->getEntityFromId(toNode->children[toPort]);

			if (!fromPortEntity || !toPortEntity)
				return;

			// --- Get slot entities safely ---
			if (fromSlotIndex >= fromPortEntity->children.size() ||
				toSlotIndex >= toPortEntity->children.size())
				return;

			Entity* fromSlotEntity = getManager()->getEntityFromId(fromPortEntity->children[fromSlotIndex]);
			Entity* toSlotEntity = getManager()->getEntityFromId(toPortEntity->children[toSlotIndex]);
			if (!fromSlotEntity || !toSlotEntity)
				return;

			// --- Retrieve positions ---
			auto& fromTransform = fromSlotEntity->GetComponent<TransformComponent>();
			auto& toTransform = toSlotEntity->GetComponent<TransformComponent>();

			glm::vec3 fromConnectionPoint = fromTransform.getPosition();
			glm::vec3 toConnectionPoint = toTransform.getPosition();

			glm::vec3 direction = toConnectionPoint - fromConnectionPoint;
			float distance = glm::length(direction);

			if (distance < 0.001f) {
				// Points are too close, skip update to avoid division by zero
				return;
			}

			glm::vec3 unitDirection = direction / distance;

			// Calculate arrowhead position with offset (same as in addArrowHead)
			float offset = 10.0f;
			glm::vec3 arrowHeadPos = toConnectionPoint - unitDirection * offset;

			// Calculate rotation based on the direction between slots
			float angleRadians = -atan2(direction.y, direction.x);

			// Update the arrowhead position and rotation
			getManager()->getEntityFromId(children[LinkChildren_ToString(ARROWHEAD)])->GetComponent<TransformComponent>().position = arrowHeadPos;
			getManager()->getEntityFromId(children[LinkChildren_ToString(ARROWHEAD)])->GetComponent<TransformComponent>().setRotation(glm::vec3(0.0f, 0.0f, angleRadians + glm::half_pi<float>()));

			// Update the arrowhead entity
			getManager()->getEntityFromId(children[LinkChildren_ToString(ARROWHEAD)])->update(0.0f);
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
		NodeEntity* fromNode = getFromNode();
		NodeEntity* toNode = getToNode();

		Entity* fromPortEntity = getManager()->getEntityFromId(fromNode->children[fromPort]);
		Entity* toPortEntity = getManager()->getEntityFromId(toNode->children[toPort]);

		// Check if slot indices are valid
		if ((fromSlotIndex >= fromPortEntity->children.size()) ||
			(toSlotIndex >= toPortEntity->children.size())) {
			TazGraphEngine::ConsoleLogger::error("Invalid slot indices!");
			return;
		}

		// Get the actual connection points from the port slots
		glm::vec3 fromConnectionPoint = getManager()->getEntityFromId(fromPortEntity->children[fromSlotIndex])->GetComponent<TransformComponent>().getPosition();
		glm::vec3 toConnectionPoint = getManager()->getEntityFromId(toPortEntity->children[toSlotIndex])->GetComponent<TransformComponent>().getPosition();

		glm::vec3 direction = toConnectionPoint - fromConnectionPoint;
		glm::vec3 unitDirection = glm::normalize(direction);

		// Calculate arrowhead position (slightly offset from the target slot)
		float offset = 10.0f;
		glm::vec3 arrowHeadPos = toConnectionPoint - unitDirection * offset;

		auto& temp_arrowHead = getManager()->addEntityNoId<Empty>();

		// Calculate rotation based on the direction between slots
		float angleRadians = -atan2(direction.y, direction.x);
		glm::vec3 arrowSize(5.0f, 10.0f, 0.0f);

		temp_arrowHead.addComponent<TransformComponent>(arrowHeadPos, arrowSize, 1);
		temp_arrowHead.addComponent<Triangle_w_Color>();
		temp_arrowHead.GetComponent<Triangle_w_Color>().color = TazColor(0, 0, 0, 255);

		// Rotate the arrowhead to point in the direction of the connection
		temp_arrowHead.GetComponent<TransformComponent>().setRotation(glm::vec3(0.0f, 0.0f, angleRadians + glm::half_pi<float>()));

		temp_arrowHead.addGroup(Manager::groupArrowHeads_0);
		temp_arrowHead.setParentEntity(this, LinkChildren_ToString(ARROWHEAD));
		manager.grid->addEmpty(&temp_arrowHead, manager.grid->getGridLevel());
		children[LinkChildren_ToString(ARROWHEAD)] = temp_arrowHead.getId();
	}

	void removeArrowHead() override {
		if (children.contains(LinkChildren_ToString(ARROWHEAD))) {
			getManager()->getEntityFromId(children[LinkChildren_ToString(ARROWHEAD)])->removeFromCell();
			getManager()->getEntityFromId(children[LinkChildren_ToString(ARROWHEAD)])->destroy();
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

			Entity* oldPortEntity = getManager()->getEntityFromId(node->children[oldPort]);
			if (oldPortEntity && oldPortEntity->hasComponent<PortComponent>()) {
				removeSlot(oldPortEntity, oldSlotIndex);
				updateLinksSlotIndices(node, oldPort, oldSlotIndex, true);
				updateLinksSlotIndices(node, oldPort, oldSlotIndex, false);
			}
		}

		if (oldPort != newPort) {
			if (std::get<std::string>(newPort).empty() || !node->children.contains(newPort)) {
				return -1;
			}

			Entity* newPortEntity = getManager()->getEntityFromId(node->children[newPort]);
			if (!newPortEntity || !newPortEntity->hasComponent<PortComponent>()) {
				return -1;
			}

			// Create new slot
			auto& newSlot = node->getManager()->addEntityNoId<Empty>();
			newSlot.addGroup(Manager::groupPortSlots);
			TransformComponent& portTransform = newPortEntity->GetComponent<TransformComponent>();
			newSlot.addComponent<TransformComponent>(
				portTransform.position,
				glm::vec3(1),
				1.0f
			);
			newSlot.addComponent<Rectangle_w_Color>();
			newSlot.GetComponent<Rectangle_w_Color>().setColor(TazColor(0, 250, 0, 255));
			newSlot.setParentEntity(newPortEntity);

			// Add to port and get the assigned index
			int newSlotIndex = addSlot(newPortEntity, newSlot.getId());
			return newSlotIndex;
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

		Entity* portEntity = getManager()->getEntityFromId(node->children[port]);
		if (portEntity && portEntity->hasComponent<PortComponent>()) {
			if (removeSlot(portEntity, slotIndex)) {
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

	std::vector<EntityID> getSlots(Entity* portEntity) const {
		std::vector<EntityID> slots;
		for (const auto& [id, child] : portEntity->children) {
			auto* ent = portEntity->getManager()->getEntityFromId(child);

			if (ent->hasComponent<PortSlotComponent>()) {
				slots.push_back(child);
			}
		}
		return slots;
	}

	// Get slot by index
	EntityID getSlotByIndex(Entity* portEntity, int index) const {
		for (const auto& [id, child] : portEntity->children) {
			auto* ent = portEntity->getManager()->getEntityFromId(child);

			if (ent->hasComponent<PortSlotComponent>() &&
				ent->GetComponent<PortSlotComponent>().index == index) {
				return child;
			}
		}
		TazGraphEngine::ConsoleLogger::error("index doesn't have entity");
		return nullptr;
	}

	// Add slot with auto-indexing
	int addSlot(Entity* portEntity, EntityID slot) {
		// Find max index
		int maxIndex = -1;
		for (const auto& [id, child] : portEntity->children) {
			auto* ent = portEntity->getManager()->getEntityFromId(child);

			if (ent->hasComponent<PortSlotComponent>()) {
				maxIndex = std::max(maxIndex, ent->GetComponent<PortSlotComponent>().index);
			}
		}
		auto* slotEnt = portEntity->getManager()->getEntityFromId(slot);

		int newIndex = maxIndex + 1;
		auto& slotComp = slotEnt->addComponent<PortSlotComponent>();
		slotComp.index = newIndex;

		portEntity->children[newIndex] = slot; // Use index as key
		return newIndex;
	}

	// Remove slot by index
	bool removeSlot(Entity* portEntity, int index) {
		for (auto it = portEntity->children.begin(); it != portEntity->children.end(); ++it) {
			EntityID child = it->second;
			auto* childEnt = portEntity->getManager()->getEntityFromId(child);

			if (childEnt->hasComponent<PortSlotComponent>() &&
				childEnt->GetComponent<PortSlotComponent>().index == index) {
				childEnt->destroy();
				portEntity->children.erase(it);
				return true;
			}
		}
		return false;
	}

	// Get number of slots
	size_t getSlotCount(Entity* portEntity) const {
		size_t count = 0;
		for (const auto& [id, child] : portEntity->children) {
			auto* childEnt = portEntity->getManager()->getEntityFromId(child);

			if (childEnt->hasComponent<PortSlotComponent>()) {
				count++;
			}
		}
		return count;
	}

};