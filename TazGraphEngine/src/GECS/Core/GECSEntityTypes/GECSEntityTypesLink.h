#pragma once

#include "./GECSEntityTypes.h"

class Link : public LinkEntity {
private:



public:

	Link(Manager& mManager) : LinkEntity(mManager) {
	}

	Link(Manager& mManager, EntityID mfromId, EntityID mtoId)
		: LinkEntity(mManager, mfromId, mtoId)
	{
	}

	Link(
		Manager& mManager,
		EntityID mfromId, EntityID mtoId,
		EntityID m_fromPort, EntityID m_toPort,
		int m_fromSlot, int m_toSlot
	)
		: LinkEntity(mManager,
			mfromId, mtoId,
			m_fromPort, m_toPort,
			m_fromSlot, m_toSlot)
	{

	}

	Link(
		Manager& mManager,
		glm::vec3 mfromPos, glm::vec3 mtoPos
	)
		: LinkEntity(mManager,
			mfromPos, mtoPos)
	{

	}
	
	Link(
		Manager& mManager,
		ConnectionType m_type
	)
		: LinkEntity(mManager)
	{
		type = m_type;
	}

	void onCreation() override {
		// Now getId() is valid
		if (type != ConnectionType::DIRECT_POSITIONS) {
			dynamic_cast<NodeEntity*>(manager.getEntityFromId(fromId))->addOutLink(getId());
			dynamic_cast<NodeEntity*>(manager.getEntityFromId(toId))->addInLink(getId());
		}
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
			NodeEntity* fromNode = dynamic_cast<NodeEntity*>(manager.getEntityFromId(getFromNode()));
			NodeEntity* toNode = dynamic_cast<NodeEntity*>(manager.getEntityFromId(getToNode()));

			const auto& fromCell = manager.grid->getCell(*fromNode, level);
			const auto& toCell = manager.grid->getCell(*toNode, level);

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
			NodeEntity* fromNode = dynamic_cast<NodeEntity*>(manager.getEntityFromId(getFromNode()));
			NodeEntity* toNode = dynamic_cast<NodeEntity*>(manager.getEntityFromId(getToNode()));
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

	void setConnectionType(ConnectionType setType) override {
		type = setType;
	}

	// ========== HELPER: Find next available slot index ==========

	int findNextAvailableSlotIndex(Entity* portEntity) {
		if (!portEntity || portEntity->children.empty()) {
			return 0;
		}

		// Find the highest existing index
		int maxIndex = -1;
		for (const auto& [slotIndex, entityId] : portEntity->children) {
			if (std::get<int>(slotIndex) > maxIndex) {
				maxIndex = std::get<int>(slotIndex);
			}
		}

		return maxIndex + 1;
	}

	// ========== HELPER: Remove slot and reindex remaining slots ==========

	void removeSlotAndReindex(Entity* portEntity, int slotIndexToRemove) {
		if (!portEntity) return;

		// Find and remove the slot
		auto it = portEntity->children.find(slotIndexToRemove);
		if (it == portEntity->children.end()) {
			return; // Slot doesn't exist
		}

		// Delete the slot entity
		Entity* slotEntity = getManager()->getEntityFromId(it->second);
		if (slotEntity) {
			slotEntity->destroy();
		}

		// Remove from map
		portEntity->children.erase(it);

		// Reindex all slots with higher indices
		std::map<EntityID, EntityID> updatedChildren;

		for (auto& [slotIndex, entityId] : portEntity->children) {
			Entity* slot = portEntity->getManager()->getEntityFromId(entityId);

			if (std::get<int>(slotIndex) > slotIndexToRemove) {
				// Shift index down by 1
				int newIndex = std::get<int>(slotIndex) - 1;

				if (slot->hasComponent<PortSlotComponent>()) {
					slot->GetComponent<PortSlotComponent>().index = newIndex;
				}

				updatedChildren[newIndex] = entityId;
			}
			else {
				// Keep same index
				updatedChildren[slotIndex] = entityId;
			}
		}

		portEntity->children = std::move(updatedChildren);
	}

	int assignSlotIndex(NodeEntity* node, EntityID newPort, EntityID oldPort, int oldSlotIndex) {
		// If port changed, remove link from old port's slots
		if (std::holds_alternative<std::string>(oldPort) &&
			!std::get<std::string>(oldPort).empty() &&
			oldPort != newPort) {

			Entity* oldPortEntity = getManager()->getEntityFromId(node->children[oldPort]);

			if (oldPortEntity && oldPortEntity->hasComponent<PortComponent>()) {
				removeSlotAndReindex(oldPortEntity, oldSlotIndex);
				updateLinksSlotIndices(node, oldPort, oldSlotIndex, true);
				updateLinksSlotIndices(node, oldPort, oldSlotIndex, false);
			}
		}

		// If port changed or first assignment, create new slot
		if (oldPort != newPort) {
			if (std::get<std::string>(newPort).empty() || !node->children.contains(newPort)) {
				return -1;
			}

			Entity* newPortEntity = getManager()->getEntityFromId(node->children[newPort]);
			if (!newPortEntity || !newPortEntity->hasComponent<PortComponent>()) {
				return -1;
			}

			// Find the next available slot index
			int newSlotIndex = findNextAvailableSlotIndex(newPortEntity);

			// Create new slot
			auto& newSlot = node->getManager()->addEntityFromParent<Empty>(newPortEntity);
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

			// Add to port with explicit index
			newPortEntity->children[newSlotIndex] = newSlot.getId();

			auto& slotComp = newSlot.addComponent<PortSlotComponent>();
			slotComp.index = newSlotIndex;

			return newSlotIndex;
		}

		return oldSlotIndex;
	}

	void updateConnection() override {

		if (type == ConnectionType::NODE_TO_NODE) {
			fromPort = -1;
			toPort = -1;
		}
		else if (type == ConnectionType::PORT_TO_PORT) {
			NodeEntity* from = dynamic_cast<NodeEntity*>(manager.getEntityFromId(getFromNode()));
			NodeEntity* to = dynamic_cast<NodeEntity*>(manager.getEntityFromId(getToNode()));

			TransformComponent* fromTR = &from->GetComponent<TransformComponent>();
			TransformComponent* toTR = &to->GetComponent<TransformComponent>();

			EntityID newFromPort = getBestPortForConnection(fromTR->getPosition(), toTR->getPosition());
			EntityID newToPort = getBestPortForConnection(toTR->getPosition(), fromTR->getPosition());

			fromSlotIndex = assignSlotIndex(from, newFromPort, fromPort, fromSlotIndex);
			toSlotIndex = assignSlotIndex(to, newToPort, toPort, toSlotIndex);

			fromPort = newFromPort;
			toPort = newToPort;
		}
		else if (type == ConnectionType::DIRECT_POSITIONS) {
			// it is as is
		}
		else if (type == ConnectionType::GHOST_PORT_TO_PORT) {
			// get adjacen path links ports and slots
			InnerLink* iL = &GetComponent<InnerLink>();

			LinkEntity* iL_1 = dynamic_cast<LinkEntity*>(manager.getEntityFromId(iL->first_pathLink));
			LinkEntity* iL_2 = dynamic_cast<LinkEntity*>(manager.getEntityFromId(iL->second_pathLink));

			fromPort = iL_1->toPort;
			toPort = iL_2->fromPort;

			fromSlotIndex = iL_1->toSlotIndex;
			toSlotIndex = iL_2->fromSlotIndex;
		}
		else {
			TazGraphEngine::ConsoleLogger::error("type doesn't exist for link");
		}

		updateConnectionPositions();
	}

	void updatePortSlots() override {
		NodeEntity* from = dynamic_cast<NodeEntity*>(manager.getEntityFromId(getFromNode()));
		NodeEntity* to = dynamic_cast<NodeEntity*>(manager.getEntityFromId(getToNode()));

		Entity* from_port = getManager()->getEntityFromId(from->children[fromPort]);
		from_port->update(0.0f);
		Entity* to_port = getManager()->getEntityFromId(to->children[toPort]);
		to_port->update(0.0f);


		Entity* from_port_slot = getManager()->getEntityFromId(from_port->children[fromSlotIndex]);
		from_port_slot->update(0.0f);
		Entity* to_port_slot = getManager()->getEntityFromId(to_port->children[toSlotIndex]);
		to_port_slot->update(0.0f);
	}


	void updateConnectionPositions() {
		if (type == ConnectionType::NODE_TO_NODE) {
			NodeEntity* from = dynamic_cast<NodeEntity*>(manager.getEntityFromId(getFromNode()));
			NodeEntity* to = dynamic_cast<NodeEntity*>(manager.getEntityFromId(getToNode()));

			fromPos = from->
				GetComponent<TransformComponent>()
				.getPosition();
			toPos = to->
				GetComponent<TransformComponent>()
				.getPosition();
		}
		else if (
			type == ConnectionType::PORT_TO_PORT ||
			type == ConnectionType::GHOST_PORT_TO_PORT
			) {
			NodeEntity* from = dynamic_cast<NodeEntity*>(manager.getEntityFromId(getFromNode()));
			NodeEntity* to = dynamic_cast<NodeEntity*>(manager.getEntityFromId(getToNode()));

			Entity* fromPortEntity = getManager()->getEntityFromId(from->children[fromPort]);
			Entity* toPortEntity = getManager()->getEntityFromId(to->children[toPort]);

			if (!fromPortEntity || !toPortEntity ||
				fromSlotIndex >= fromPortEntity->children.size() ||
				toSlotIndex >= toPortEntity->children.size()) {
				TazGraphEngine::ConsoleLogger::error(type == ConnectionType::PORT_TO_PORT ?
					"updateConnectionPositions port-port"
					: "updateConnectionPositions Ghost port-port");
			}

			fromPos = getManager()->getEntityFromId(fromPortEntity->children[fromSlotIndex])
				->GetComponent<TransformComponent>().getPosition();
			toPos = getManager()->getEntityFromId(toPortEntity->children[toSlotIndex])
				->GetComponent<TransformComponent>().getPosition();
		}
		else if (type == ConnectionType::DIRECT_POSITIONS) {
			// it is as is
		}
	}

	void addArrowHead() override {
		NodeEntity* fromNode = dynamic_cast<NodeEntity*>(manager.getEntityFromId(getFromNode()));
		NodeEntity* toNode = dynamic_cast<NodeEntity*>(manager.getEntityFromId(getToNode()));


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

		float linkWidth = this->GetComponent<Line_w_Color>().width;

		float arrowheadWidth = linkWidth * 2.0f;   // Arrowhead width = 2x link width
		float arrowheadHeight = arrowheadWidth * 2.0f; // Height = 2x width for good proportion

		// Calculate arrowhead position (slightly offset from the target slot)
		float offset = arrowheadHeight * 0.6f;
		glm::vec3 arrowHeadPos = toConnectionPoint - unitDirection * offset;

		auto& temp_arrowHead = getManager()->addEntityFromParent<Empty>(this, LinkChildren_ToString(ARROWHEAD));

		// Calculate rotation based on the direction between slots
		float angleRadians = -atan2(direction.y, direction.x);
		glm::vec3 arrowSize(arrowheadWidth, arrowheadHeight, 0.0f);

		temp_arrowHead.addComponent<TransformComponent>(arrowHeadPos, arrowSize, 1);
		temp_arrowHead.addComponent<Triangle_w_Color>();
		temp_arrowHead.GetComponent<Triangle_w_Color>().color = TazColor(0, 0, 0, 255);

		// Rotate the arrowhead to point in the direction of the connection
		temp_arrowHead.GetComponent<TransformComponent>().setRotation(glm::vec3(0.0f, 0.0f, angleRadians + glm::half_pi<float>()));

		temp_arrowHead.addGroup(Manager::groupArrowHeads_0);
		temp_arrowHead.setParentEntity(this);
		children[LinkChildren_ToString(ARROWHEAD)] = temp_arrowHead.getId();
	}

	void removeArrowHead() override {
		if (children.contains(LinkChildren_ToString(ARROWHEAD))) {
			getManager()->getEntityFromId(children[LinkChildren_ToString(ARROWHEAD)])->removeFromCell();
			getManager()->getEntityFromId(children[LinkChildren_ToString(ARROWHEAD)])->destroy();
			children.erase(LinkChildren_ToString(ARROWHEAD));
		}
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


	void updateLinksSlotIndices(NodeEntity* node, EntityID portIndex, int removedSlotIndex, bool isFrom) {

		// reassign slots to links
		if (isFrom) {
			for (auto& linkEntityId : node->getOutLinks()) {
				auto* linkEntity = dynamic_cast<LinkEntity*>(manager.getEntityFromId(linkEntityId));

				if (EntityIDUtils::areEqual(linkEntity->fromPort, portIndex) &&
					linkEntity->fromSlotIndex > removedSlotIndex &&
					linkEntity->type != ConnectionType::GHOST_PORT_TO_PORT) {
					linkEntity->fromSlotIndex--;
				}
			}
		}
		else {
			for (auto& linkEntityId : node->getInLinks()) {
				auto* linkEntity = dynamic_cast<LinkEntity*>(manager.getEntityFromId(linkEntityId));

				if (EntityIDUtils::areEqual(linkEntity->toPort, portIndex) &&
					linkEntity->toSlotIndex > removedSlotIndex &&
					linkEntity->type != ConnectionType::GHOST_PORT_TO_PORT) {
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

		NodeEntity* fromNode = dynamic_cast<NodeEntity*>(manager.getEntityFromId(getFromNode()));
		NodeEntity* toNode = dynamic_cast<NodeEntity*>(manager.getEntityFromId(getToNode()));


		glm::vec2 fromNodePosition = fromNode->GetComponent<TransformComponent>().getPosition();
		glm::vec2 toNodePosition = toNode->GetComponent<TransformComponent>().getPosition();

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

		NodeEntity* from = dynamic_cast<NodeEntity*>(manager.getEntityFromId(getFromNode()));
		NodeEntity* to = dynamic_cast<NodeEntity*>(manager.getEntityFromId(getToNode()));


		if (from) {
			from->removeOutLink(getId());
			removeSlotFromNode(from, fromPort, fromSlotIndex, true);
		}
		if (to) {
			to->removeInLink(getId());
			removeSlotFromNode(to, toPort, toSlotIndex, false);
		}

		removeArrowHead();
		setConnectionType(LinkEntity::ConnectionType::NODE_TO_NODE);
		updateConnection();

		manager.aboutTo_updateActiveEntities();//? cant have it at destroy in baseclass
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