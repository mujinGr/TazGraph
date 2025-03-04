#pragma once

#include "GECSManager.h"
#include "Components.h"

class LinkEntity;

typedef class EmptyEntity : public Entity {
private:
	Entity* parent_entity = nullptr;
public:
	Cell* ownerCell = nullptr;

	EmptyEntity(Manager& mManager) : Entity(mManager) {

	}
	virtual ~EmptyEntity() {

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
				manager.grid->addNode(this, newCell);
			}
		}
	}

	void removeFromCell() override {
		if (this->ownerCell) {
			removeEntity();
			this->ownerCell = nullptr;
		}
	}

	void removeEntity() {
		ownerCell->nodes.erase(
			std::remove(this->ownerCell->nodes.begin(), this->ownerCell->nodes.end(),
				this),
			this->ownerCell->nodes.end());
	}

	void setOwnerCell(Cell* cell) override {
		this->ownerCell = cell;
	}

	Cell* getOwnerCell() const override { return ownerCell; }

	Entity* getParentEntity() override {
		return parent_entity;
	}

	void setParentEntity(Entity* pEntity) override {
		parent_entity = pEntity;
	}

	void imgui_print() override {
		glm::vec2 position = this->GetComponent<TransformComponent>().getPosition();  // Make sure Entity class has a getPosition method
		ImGui::Text("Position: (%.2f, %.2f)", position.x, position.y);
	}

	void destroy() {
		Entity::destroy();
		manager.aboutTo_updateActiveEntities(); // cant have it at destroy in baseclass
	}
} Empty;

typedef class NodeEntity : public Entity {
private:
	Entity* parent_entity = nullptr;
	std::vector<Entity*> inLinks;
	std::vector<Entity*> outLinks;

	std::vector<std::string> messageLog;
public:
	Cell* ownerCell = nullptr;

	NodeEntity(Manager& mManager) : Entity(mManager) {

		auto& leftPort = mManager.addEntityNoId<Empty>();
		leftPort.addComponent<TransformComponent>().bodyDims.w = 0;
		leftPort.GetComponent<TransformComponent>().bodyDims.h = 0;
		children["leftPort"] = &leftPort;

		auto& rightPort = mManager.addEntityNoId<Empty>();
		rightPort.addComponent<TransformComponent>().bodyDims.w = 0;
		rightPort.GetComponent<TransformComponent>().bodyDims.h = 0;
		children["rightPort"] = &rightPort;

		// Initialize top port
		auto& topPort = mManager.addEntityNoId<Empty>();
		topPort.addComponent<TransformComponent>().bodyDims.w = 0;
		topPort.GetComponent<TransformComponent>().bodyDims.h = 0;
		children["topPort"] = &topPort;

		// Initialize bottom port
		auto& bottomPort = mManager.addEntityNoId<Empty>();
		bottomPort.addComponent<TransformComponent>().bodyDims.w = 0;
		bottomPort.GetComponent<TransformComponent>().bodyDims.h = 0;
		children["bottomPort"] = &bottomPort;
	}
	virtual ~NodeEntity() {

	}

	void update(float deltaTime)
	{
		cellUpdate();

		Entity::update(deltaTime);
	}

	void cellUpdate() override{
		if (this->ownerCell) {
			if (!SDL_FRectEquals(&this->GetComponent<TransformComponent>().last_bodyDims, &this->GetComponent<TransformComponent>().bodyDims))
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

	void removeFromCell() override {
		if (this->ownerCell) {
			removeEntity();
			this->ownerCell = nullptr;
		}
	}

	void removeEntity() {
		ownerCell->nodes.erase(
			std::remove(this->ownerCell->nodes.begin(), this->ownerCell->nodes.end(),
				this),
			this->ownerCell->nodes.end());
	}

	void setOwnerCell(Cell* cell) override {
		this->ownerCell = cell;
	}

	Cell* getOwnerCell() const override { return ownerCell; }

	Entity* getParentEntity() override {
		return parent_entity;
	}

	void setParentEntity(Entity* pEntity) override {
		parent_entity = pEntity;
	}

	void addInLink(Entity* link) {
		inLinks.push_back(link);
	}

	void addOutLink(Entity* link) {
		outLinks.push_back(link);
	}

	const std::vector<Entity*>& getInLinks() const override {
		return inLinks;
	}

	const std::vector<Entity*>& getOutLinks() const override {
		return outLinks;
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
} Node;


typedef class LinkEntity : public Entity {
private:

	unsigned int fromId = 0;
	unsigned int toId = 0;

	Node* from = nullptr;
	Node* to = nullptr;

public:
	std::string fromPort;
	std::string toPort;

	std::vector<Cell*> ownerCells = {};

	Color color = {};

	LinkEntity(Manager& mManager) : Entity(mManager) {
	}

	LinkEntity(Manager& mManager, unsigned int mfromId, unsigned int mtoId)
		: Entity(mManager),
		fromId(mfromId),
		toId(mtoId)
	{
		from = dynamic_cast<Node*>(mManager.getEntityFromId(fromId));
		from->addOutLink(this);
		to = dynamic_cast<Node*>(mManager.getEntityFromId(toId));
		to->addInLink(this);

		// add arrow head
		TransformComponent* toTR = &to->GetComponent<TransformComponent>();
		TransformComponent* fromTR = &from->GetComponent<TransformComponent>();

		fromPort = getBestPortForConnection(fromTR->getCenterTransform(), toTR->getCenterTransform());
		toPort = getBestPortForConnection(toTR->getCenterTransform(), fromTR->getCenterTransform());

		TransformComponent* toPortTR = &to->children[toPort]->GetComponent<TransformComponent>();
		TransformComponent* fromPortTR = &from->children[fromPort]->GetComponent<TransformComponent>();

		glm::vec3 direction = toPortTR->getCenterTransform() - fromPortTR->getCenterTransform();

		glm::vec3 unitDirection = glm::normalize(direction);
		float offset = toTR->bodyDims.w + 5.0f;

		glm::vec3 arrowHeadPos = toPortTR->getCenterTransform() - unitDirection * offset;
		
		auto& temp_arrowHead = mManager.addEntity<Empty>();


		glm::vec3 farrowSize(10.0f, 20.0f, 0.0f);

		temp_arrowHead.addComponent<TransformComponent>(arrowHeadPos - (farrowSize /2.0f), Manager::actionLayer, farrowSize, 1);
		temp_arrowHead.addComponent<Triangle_w_Color>();
		temp_arrowHead.GetComponent<Triangle_w_Color>().color = Color(0, 0, 0, 255);

		temp_arrowHead.addGroup(Manager::groupArrowHeads_0);

		// Calculate the angle in radians, and convert it to degrees
		float angleRadians = atan2(direction.y, direction.x);
		float angleDegrees = glm::degrees(angleRadians);


		temp_arrowHead.GetComponent<TransformComponent>().setRotation(glm::vec3(0.0f, 0.0f, angleDegrees + 90.0f));

		temp_arrowHead.setParentEntity(this);
		children["ArrowHead"] = &temp_arrowHead;

		mManager.grid->addNode(&temp_arrowHead, mManager.grid->getGridLevel());
	}

	LinkEntity(Manager& mManager, Entity* mfrom, Entity* mto)
		: Entity(mManager),
		from(dynamic_cast<Node*>(mfrom)),
		to(dynamic_cast<Node*>(mto))
	{
		fromId = from->getId();
		toId = to->getId();
	}

	virtual ~LinkEntity() {

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
			removeFromCell();
			std::vector<Cell*> cells = manager.grid->getLinkCells(*this, manager.grid->getGridLevel());
			for (auto& cell : cells) {
				manager.grid->addLink(this, cell);
			}
		}
	}

	void removeFromCell() override {
		removeEntity();
		ownerCells.clear();
	}

	void removeEntity() {
		for (auto cell : ownerCells) {
			cell->links.erase(std::remove(cell->links.begin(), cell->links.end(),
				this),
				cell->links.end());
		}
	}

	void setOwnerCell(Cell* cell) override {
		this->ownerCells.push_back(cell);
	}

	Cell* getOwnerCell() const override { return ownerCells[0]; }

	Entity* getFromNode() const override {
		return from;
	}

	Entity* getToNode() const override {
		return to;
	}

	void updateLinkPorts() override{
		TransformComponent* toTR = &to->GetComponent<TransformComponent>();
		TransformComponent* fromTR = &from->GetComponent<TransformComponent>();

		fromPort = getBestPortForConnection(fromTR->getCenterTransform(), toTR->getCenterTransform());
		toPort = getBestPortForConnection(toTR->getCenterTransform(), fromTR->getCenterTransform());


		if (children["ArrowHead"]) {
			TransformComponent* tr = &children["ArrowHead"]->GetComponent<TransformComponent>();
			
			children["ArrowHead"]->update(0.0f);

			// set position of arrowHead

			TransformComponent* toPortTR = &to->children[toPort]->GetComponent<TransformComponent>();
			TransformComponent* fromPortTR = &from->children[fromPort]->GetComponent<TransformComponent>();

			glm::vec3 direction = toPortTR->getCenterTransform() - fromPortTR->getCenterTransform();

			glm::vec3 unitDirection = glm::normalize(direction);
			float offset = 10.0f;

			glm::vec3 arrowHeadPos = toPortTR->getCenterTransform() - unitDirection * offset;

			// Calculate the angle in radians, and convert it to degrees
			float angleRadians = atan2(direction.y, direction.x);
			float angleDegrees = glm::degrees(angleRadians);

			glm::ivec3 arrowSize(10, 20, 0);
			glm::vec3 farrowSize(10.0f, 20.0f, 0.0f);

			glm::vec3 newArrowHeadPosition = arrowHeadPos - (farrowSize / 2.0f);
			children["ArrowHead"]->GetComponent<TransformComponent>().setPosition_X(newArrowHeadPosition.x);
			children["ArrowHead"]->GetComponent<TransformComponent>().setPosition_Y(newArrowHeadPosition.y);

			children["ArrowHead"]->GetComponent<TransformComponent>().setRotation(glm::vec3(0.0f,0.0f,angleDegrees + 90.0f));
		}
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

	Entity* getFromPort() override {
		return from->children[fromPort];
	}

	Entity* getToPort()  override{
		return to->children[toPort];
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

} Link;