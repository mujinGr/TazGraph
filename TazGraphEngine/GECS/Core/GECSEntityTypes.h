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
		cellUpdate();

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

		//auto& leftPort = mManager.addEntityNoId<Empty>();
		//leftPort.addComponent<TransformComponent>().size.x = 0;
		//leftPort.GetComponent<TransformComponent>().size.y = 0;
		//children["leftPort"] = &leftPort;

		//auto& rightPort = mManager.addEntityNoId<Empty>();
		//rightPort.addComponent<TransformComponent>().size.x = 0;
		//rightPort.GetComponent<TransformComponent>().size.y = 0;
		//children["rightPort"] = &rightPort;

		//// Initialize top port
		//auto& topPort = mManager.addEntityNoId<Empty>();
		//topPort.addComponent<TransformComponent>().size.x = 0;
		//topPort.GetComponent<TransformComponent>().size.y = 0;
		//children["topPort"] = &topPort;

		//// Initialize bottom port
		//auto& bottomPort = mManager.addEntityNoId<Empty>();
		//bottomPort.addComponent<TransformComponent>().size.x = 0;
		//bottomPort.GetComponent<TransformComponent>().size.y = 0;
		//children["bottomPort"] = &bottomPort;
	}

	void addGroup(Group mGroup) override {
		Entity::addGroup(mGroup);
		manager.AddToGroup(this, mGroup);
	}

	virtual ~Node() {

	}

	void update(float deltaTime)
	{
		cellUpdate();

		Entity::update(deltaTime);
	}

	void cellUpdate() override{
		if (this->ownerCell) {
			if (this->GetComponent<TransformComponent>().last_position != this->GetComponent<TransformComponent>().position)
			{
				Cell* newCell = manager.grid->getCell(*this, manager.grid->getGridLevel());
				if (newCell != this->ownerCell) {
					// Need to shift the entity
					removeEntity();
					manager.grid->addNode(this, newCell);

					// not important for precision, it is fine to do it only when the cell changes
					// now updateCells of all the in and out links
					for (auto& link : inLinks) {
						link->cellUpdate();
					}
					for (auto& link : outLinks) {
						link->cellUpdate();
					}

				}
				for (auto& link : inLinks) {
					link->updateLinkPorts();
				}
				for (auto& link : outLinks) {
					link->updateLinkPorts();
				}
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

		// add arrow head
		TransformComponent* toTR = &to->GetComponent<TransformComponent>();
		TransformComponent* fromTR = &from->GetComponent<TransformComponent>();

		/*fromPort = getBestPortForConnection(fromTR->getCenterTransform(), toTR->getCenterTransform());
		toPort = getBestPortForConnection(toTR->getCenterTransform(), fromTR->getCenterTransform());*/

		//TransformComponent* toPortTR = &to->children[toPort]->GetComponent<TransformComponent>();
		//TransformComponent* fromPortTR = &from->children[fromPort]->GetComponent<TransformComponent>();

		//glm::vec3 direction = toPortTR->getCenterTransform() - fromPortTR->getCenterTransform();

		//glm::vec3 unitDirection = glm::normalize(direction);
		//float offset = toTR->size.x + 5.0f;

		//glm::vec3 arrowHeadPos = toPortTR->getCenterTransform() - unitDirection * offset;
		//
		//auto& temp_arrowHead = mManager.addEntity<Empty>();


		//glm::vec3 farrowSize(10.0f, 20.0f, 0.0f);

		//temp_arrowHead.addComponent<TransformComponent>(arrowHeadPos - (farrowSize /2.0f), Layer::action, farrowSize, 1);
		//temp_arrowHead.addComponent<Triangle_w_Color>();
		//temp_arrowHead.GetComponent<Triangle_w_Color>().color = Color(0, 0, 0, 255);

		//temp_arrowHead.addGroup(Manager::groupArrowHeads_0);

		//// Calculate the angle in radians, and convert it to degrees
		//float angleRadians = atan2(direction.y, direction.x);
		//float angleDegrees = glm::degrees(angleRadians);


		//temp_arrowHead.GetComponent<TransformComponent>().setRotation(glm::vec3(0.0f, 0.0f, angleDegrees + 90.0f));

		//temp_arrowHead.setParentEntity(this);
		//children["ArrowHead"] = &temp_arrowHead;

		//mManager.grid->addEmpty(&temp_arrowHead, mManager.grid->getGridLevel());
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
		if (manager.grid->getCell(*getFromNode(), manager.grid->getGridLevel()) != ownerCells.front()
			|| manager.grid->getCell(*getToNode(), manager.grid->getGridLevel()) != ownerCells.back())
		{
			removeFromCells();
			
			manager.grid->addLink(this, manager.grid->getGridLevel());
		}
	}

	

	void updateLinkPorts() override{
		TransformComponent* toTR = &to->GetComponent<TransformComponent>();
		TransformComponent* fromTR = &from->GetComponent<TransformComponent>();

		/*fromPort = getBestPortForConnection(fromTR->getCenterTransform(), toTR->getCenterTransform());
		toPort = getBestPortForConnection(toTR->getCenterTransform(), fromTR->getCenterTransform());*/


		//if (children["ArrowHead"]) {
		//	TransformComponent* tr = &children["ArrowHead"]->GetComponent<TransformComponent>();
		//	
		//	children["ArrowHead"]->update(0.0f);

		//	// set position of arrowHead

		//	TransformComponent* toPortTR = &to->children[toPort]->GetComponent<TransformComponent>();
		//	TransformComponent* fromPortTR = &from->children[fromPort]->GetComponent<TransformComponent>();

		//	glm::vec3 direction = toPortTR->getCenterTransform() - fromPortTR->getCenterTransform();

		//	glm::vec3 unitDirection = glm::normalize(direction);
		//	float offset = 10.0f;

		//	glm::vec3 arrowHeadPos = toPortTR->getCenterTransform() - unitDirection * offset;

		//	// Calculate the angle in radians, and convert it to degrees
		//	float angleRadians = atan2(direction.y, direction.x);
		//	float angleDegrees = glm::degrees(angleRadians);

		//	glm::ivec3 arrowSize(10, 20, 0);
		//	glm::vec3 farrowSize(10.0f, 20.0f, 0.0f);

		//	glm::vec3 newArrowHeadPosition = arrowHeadPos - (farrowSize / 2.0f);
		//	children["ArrowHead"]->GetComponent<TransformComponent>().setPosition_X(newArrowHeadPosition.x);
		//	children["ArrowHead"]->GetComponent<TransformComponent>().setPosition_Y(newArrowHeadPosition.y);

		//	children["ArrowHead"]->GetComponent<TransformComponent>().setRotation(glm::vec3(0.0f,0.0f,angleDegrees + 90.0f));
		//}
	}

	std::string getBestPortForConnection(const glm::vec2& fromPos, const glm::vec2& toPos) {
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