#pragma once

#include "GECSManager.h"
#include "Components.h"

class LinkEntity;

typedef class NodeEntity : public Entity {
private:
	Entity* parent_entity = nullptr;
	std::vector<LinkEntity*> inLinks;
	std::vector<LinkEntity*> outLinks;

public:
	Cell* ownerCell = nullptr;

	NodeEntity(Manager& mManager) : Entity(mManager) {

	}
	virtual ~NodeEntity() {

	}

	void update(float deltaTime)
	{
		Entity::update(deltaTime);
		// after cellUpdate then update the links
		/*for (auto link : inLinks) {
			link->update(deltaTime);
		}
		for (auto link : outLinks) {
			link->update(deltaTime);
		}*/
	}

	void cellUpdate() override{
		if (this->ownerCell) {
			Cell* newCell = manager.grid->getCell(*this);
			if (newCell != this->ownerCell) {
				// Need to shift the entity
				removeEntity();
				manager.grid->addEntity(this, newCell);
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
		ownerCell->entities.erase(
			std::remove(this->ownerCell->entities.begin(), this->ownerCell->entities.end(),
				this),
			this->ownerCell->entities.end());
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

	void addInLink(LinkEntity* link) {
		inLinks.push_back(link);
	}

	void addOutLink(LinkEntity* link) {
		outLinks.push_back(link);
	}

	const std::vector<LinkEntity*>& getInLinks() const {
		return inLinks;
	}

	const std::vector<LinkEntity*>& getOutLinks() const {
		return outLinks;
	}

	void imgui_print() override {
		glm::vec2 position = this->GetComponent<TransformComponent>().getPosition();  // Make sure Entity class has a getPosition method
		ImGui::Text("Position: (%.2f, %.2f)", position.x, position.y);
	}
	

} Node;


typedef class LinkEntity : public Entity {
private:

	unsigned int fromId = 0;
	unsigned int toId = 0;

	Node* from = nullptr;
	Node* to = nullptr;

public:
	std::vector<Cell*> ownerCells = {};

	Color color = {};

	LinkEntity(Manager& mManager) : Entity(mManager) {
	}

	LinkEntity(Manager& mManager, unsigned int mfromId, unsigned int mtoId)
		: Entity(mManager),
		fromId(mfromId),
		toId(mtoId)
	{
		from = dynamic_cast<Node*>(&mManager.getEntityFromId(fromId));
		from->addOutLink(this);
		to = dynamic_cast<Node*>(&mManager.getEntityFromId(toId));
		to->addInLink(this);
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
		if (manager.grid->getCell(*getFromNode()) != ownerCells.front()
			|| manager.grid->getCell(*getToNode()) != ownerCells.back())
		{
			removeFromCell();
			std::vector<Cell*> cells = manager.grid->getLinkCells(*this);
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

	Node* getFromNode() const override {
		return from;
	}

	Node* getToNode() const override {
		return to;
	}

	void imgui_print() override {
		glm::vec2 fromNodePosition = this->getFromNode()->GetComponent<TransformComponent>().getCenterTransform();
		glm::vec2 toNodePosition = this->getToNode()->GetComponent<TransformComponent>().getCenterTransform();

		ImGui::Text("From Node Position: (%.2f, %.2f)", fromNodePosition.x, fromNodePosition.y);
		ImGui::Text("To Node Position: (%.2f, %.2f)", toNodePosition.x, toNodePosition.y);

		ImGui::Text("Bounding boxes of intercepted cells:");

		for (auto cell : ownerCells) {
			ImGui::Text("- %.2f , %.2f", cell->boundingBox.x, cell->boundingBox.y);
		}
	}

} Link;