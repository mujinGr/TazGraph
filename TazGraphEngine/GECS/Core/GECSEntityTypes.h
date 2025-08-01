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

class Node: public NodeEntity {
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

		glm::vec3 m_position = glm::vec3(-tr->size.x / 2, 0.0f, 0.0f);

		if (children["leftPort"]) {
			children["leftPort"]->GetComponent<TransformComponent>().position = m_position;
			children["leftPort"]->update(deltaTime);
		}

		m_position = glm::vec3(tr->size.x / 2, 0.0f, 0.0f);

		if (children["rightPort"]) {
			children["rightPort"]->GetComponent<TransformComponent>().position = m_position;
			children["rightPort"]->update(deltaTime);
		}
		
		m_position = glm::vec3(0.0f, -tr->size.y / 2.0f, 0.0f);

		if (children["topPort"]) {
			children["topPort"]->GetComponent<TransformComponent>().position = m_position;
			children["topPort"]->update(deltaTime);
		}
		
		m_position = glm::vec3(0.0f, tr->size.y / 2.0f, 0.0f);

		if (children["bottomPort"]) {
			children["bottomPort"]->GetComponent<TransformComponent>().position = m_position;
			children["bottomPort"]->update(deltaTime);
		}
	}

	void cellUpdate() override{
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

	void addMessage(std::string mMessage) override{
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
		TransformComponent* tr = &GetComponent<TransformComponent>();

		auto& leftPort = getManager()->addEntityNoId<Empty>();
		glm::vec3 m_position = glm::vec3(-tr->size.x / 2, 0.0f, 0.0f);
		leftPort.addComponent<TransformComponent>(m_position, glm::vec3(0), 1.0f);
		children["leftPort"] = &leftPort;
		children["leftPort"]->setParentEntity(this);
		children["leftPort"]->GetComponent<TransformComponent>().bodyCenter = tr->bodyCenter + m_position;

		auto& rightPort = getManager()->addEntityNoId<Empty>();
		m_position = glm::vec3(tr->size.x / 2 , 0.0f, 0.0f);
		rightPort.addComponent<TransformComponent>(m_position, glm::vec3(0), 1.0f);
		children["rightPort"] = &rightPort;
		children["rightPort"]->setParentEntity(this);
		children["rightPort"]->GetComponent<TransformComponent>().bodyCenter = tr->bodyCenter + m_position;

		// Initialize top port
		auto& topPort = getManager()->addEntityNoId<Empty>();
		m_position = glm::vec3(0.0f, -tr->size.y / 2.0f, 0.0f);
		topPort.addComponent<TransformComponent>(m_position, glm::vec3(0), 1.0f);
		children["topPort"] = &topPort;
		children["topPort"]->setParentEntity(this);
		children["topPort"]->GetComponent<TransformComponent>().bodyCenter = tr->bodyCenter + m_position;

		// Initialize bottom port
		auto& bottomPort = getManager()->addEntityNoId<Empty>();
		m_position = glm::vec3(0.0f, tr->size.y / 2.0f, 0.0f);
		bottomPort.addComponent<TransformComponent>(m_position, glm::vec3(0), 1.0f);
		children["bottomPort"] = &bottomPort;
		children["bottomPort"]->setParentEntity(this);
		children["bottomPort"]->GetComponent<TransformComponent>().bodyCenter = tr->bodyCenter + m_position;

	}

	void removePorts() {
		for (auto portName : { "leftPort", "rightPort", "topPort", "bottomPort" }) {
			if (children[portName]) {
				children.erase(portName);
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

	Link(Manager& mManager, unsigned int mfromId, unsigned int mtoId)
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
		unsigned int mfromId, unsigned int mtoId,
		NodeEntity* mfrom, NodeEntity* mto
	)
		: LinkEntity(mManager,
			mfromId, mtoId,
			mfrom, mto)
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
		if (children["ArrowHead"]) {
			TransformComponent* tr = &children["ArrowHead"]->GetComponent<TransformComponent>();

			children["ArrowHead"]->update(0.0f);

			// set position of arrowHead
			TransformComponent* ch_tr = &to->children[toPort]->GetComponent<TransformComponent>();

			TransformComponent* toPortTR = ch_tr;
			TransformComponent* fromPortTR = &from->children[fromPort]->GetComponent<TransformComponent>();

			glm::vec3 direction = toPortTR->getCenterTransform() - fromPortTR->getCenterTransform();

			glm::vec3 unitDirection = glm::normalize(direction);
			float offset = 10.0f;

			glm::vec3 arrowHeadPos = toPortTR->getCenterTransform() - unitDirection * offset;

			// Calculate the angle in radians, and convert it to degrees
			float angleRadians = -atan2(direction.y, direction.x);
			float angleDegrees = glm::degrees(angleRadians);

			glm::ivec3 arrowSize(10, 20, 0);
			glm::vec3 farrowSize(10.0f, 20.0f, 0.0f);

			glm::vec3 newArrowHeadPosition = arrowHeadPos - (farrowSize / 2.0f);
			children["ArrowHead"]->GetComponent<TransformComponent>().position = newArrowHeadPosition;

			children["ArrowHead"]->GetComponent<TransformComponent>().setRotation(glm::vec3(0.0f, 0.0f, angleRadians + glm::half_pi<float>()));
		}
	}

	void updateLinkToPorts() override{
		TransformComponent* toTR = &to->GetComponent<TransformComponent>();
		TransformComponent* fromTR = &from->GetComponent<TransformComponent>();

		fromPort = getBestPortForConnection(fromTR->getCenterTransform(), toTR->getCenterTransform());
		toPort = getBestPortForConnection(toTR->getCenterTransform(), fromTR->getCenterTransform());
	}

	void addArrowHead() override {
		TransformComponent* toTR = &to->GetComponent<TransformComponent>();
		TransformComponent* fromTR = &from->GetComponent<TransformComponent>();

		fromPort = getBestPortForConnection(fromTR->getCenterTransform(), toTR->getCenterTransform());
		toPort = getBestPortForConnection(toTR->getCenterTransform(), fromTR->getCenterTransform());

		TransformComponent* toPortTR = &to->children[toPort]->GetComponent<TransformComponent>();
		TransformComponent* fromPortTR = &from->children[fromPort]->GetComponent<TransformComponent>();

		glm::vec3 direction = toPortTR->getCenterTransform() - fromPortTR->getCenterTransform();

		glm::vec3 unitDirection = glm::normalize(direction);
		float offset = 10.0f;

		glm::vec3 arrowHeadPos = toPortTR->getCenterTransform() - unitDirection * offset;
		
		auto& temp_arrowHead = getManager()->addEntityNoId<Empty>();

		// Calculate the angle in radians, and convert it to degrees
		float angleRadians = -atan2(direction.y, direction.x);
		float angleDegrees = glm::degrees(angleRadians);

		glm::vec3 farrowSize(10.0f, 20.0f, 0.0f);

		temp_arrowHead.addComponent<TransformComponent>(arrowHeadPos - (farrowSize /2.0f), farrowSize, 1);
		temp_arrowHead.addComponent<Triangle_w_Color>();
		temp_arrowHead.GetComponent<Triangle_w_Color>().color = Color(0, 0, 0, 255);

		temp_arrowHead.GetComponent<TransformComponent>().setRotation(glm::vec3(0.0f, 0.0f, angleRadians + glm::half_pi<float>()));

		temp_arrowHead.addGroup(Manager::groupArrowHeads_0);

		temp_arrowHead.setParentEntity(this);

		manager.grid->addEmpty(&temp_arrowHead, manager.grid->getGridLevel());

		children["ArrowHead"] = &temp_arrowHead;
	}

	void removeArrowHead() override {
		std::string portName = "ArrowHead";
		if (children[portName]) {
			children[portName]->removeFromCell();
			children[portName]->destroy();
			children.erase(portName);
		}
	}

	void updateLinkToNodes() override {
		fromPort = "";
		toPort = "";
	}

	std::string getBestPortForConnection(const glm::vec3& fromPos, const glm::vec3& toPos) {
		// Simple logic to determine the port based on relative position
		float deltaX = toPos.x - fromPos.x;
		float deltaY = toPos.y - fromPos.y;

		if (abs(deltaX) > abs(deltaY)) {  // Horizontal distance is greater
			return deltaX > 0 ? "rightPort" : "leftPort";
		}
		else {  // Vertical distance is greater
			return deltaY > 0 ? "bottomPort" : "topPort";
		}
	}

	


	void imgui_print() override {
		glm::vec2 fromNodePosition = this->getFromNode()->GetComponent<TransformComponent>().getCenterTransform();
		glm::vec2 toNodePosition = this->getToNode()->GetComponent<TransformComponent>().getCenterTransform();

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

		if (children["ArrowHead"]) {
			children["ArrowHead"]->destroy();
		}

		manager.aboutTo_updateActiveEntities();
	}

};