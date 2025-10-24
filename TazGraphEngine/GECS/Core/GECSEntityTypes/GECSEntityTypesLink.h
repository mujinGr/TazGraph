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

	void updateConnection(ConnectionType setType) override {
		type = setType;

		if (type == ConnectionType::NODE_TO_NODE) {
			fromPort = -1;
			toPort = -1;
		}
		else if (type == ConnectionType::PORT_TO_PORT) {
			NodeEntity* from = dynamic_cast<NodeEntity*>(manager.getEntityFromId(getFromNode()));
			NodeEntity* to = dynamic_cast<NodeEntity*>(manager.getEntityFromId(getToNode()));

			TransformComponent* toTR = &to->GetComponent<TransformComponent>();
			TransformComponent* fromTR = &from->GetComponent<TransformComponent>();

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
		updateConnectionPositions();
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
		else if (type == ConnectionType::PORT_TO_PORT) {
			NodeEntity* from = dynamic_cast<NodeEntity*>(manager.getEntityFromId(getFromNode()));
			NodeEntity* to = dynamic_cast<NodeEntity*>(manager.getEntityFromId(getToNode()));

			Entity* fromPortEntity = getManager()->getEntityFromId(from->children[fromPort]);
			Entity* toPortEntity = getManager()->getEntityFromId(to->children[toPort]);

			if (!fromPortEntity || !toPortEntity ||
				fromSlotIndex >= fromPortEntity->children.size() ||
				toSlotIndex >= toPortEntity->children.size()) {
				TazGraphEngine::ConsoleLogger::error("updateConnectionPositions port-port");
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
			for (auto& linkEntityId : node->getOutLinks()) {
				auto* linkEntity = dynamic_cast<LinkEntity*>(manager.getEntityFromId(linkEntityId));
				if (EntityIDUtils::areEqual(linkEntity->fromPort, portIndex) &&
					linkEntity->fromSlotIndex > removedSlotIndex) {
					linkEntity->fromSlotIndex--;
				}
			}
		}
		else {
			for (auto& linkEntityId : node->getInLinks()) {
				auto* linkEntity = dynamic_cast<LinkEntity*>(manager.getEntityFromId(linkEntityId));

				if (EntityIDUtils::areEqual(linkEntity->toPort, portIndex) &&
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
			from->removeOutLink(id);
			removeSlotFromNode(from, fromPort, fromSlotIndex, true);
		}
		if (to) {
			to->removeInLink(id);
			removeSlotFromNode(to, toPort, toSlotIndex, false);
		}

		removeArrowHead();
		updateConnection(LinkEntity::ConnectionType::NODE_TO_NODE);

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