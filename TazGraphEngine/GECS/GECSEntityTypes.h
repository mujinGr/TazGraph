#pragma once

#include "GECSManager.h"
#include "Components.h"

typedef class LinkEntity : public Entity {
private:

	unsigned int fromId = 0;
	unsigned int toId = 0;

	Entity* from = nullptr;
	Entity* to = nullptr;

public:
	std::vector<Cell*> ownerCells = {};

	LinkEntity(Manager& mManager) : Entity(mManager) {
	}

	LinkEntity(Manager& mManager, unsigned int mfromId, unsigned int mtoId) 
		: Entity(mManager),
		fromId(mfromId),
		toId(mtoId)
	{
		from = &mManager.getEntityFromId(fromId);
		to = &mManager.getEntityFromId(toId);
	}

	~LinkEntity() {

	}

	void cellUpdate() override {

	}

	void removeFromCell() override {
		for (auto cell : ownerCells) {
			if (cell) {
				cell->entities.erase(
					std::remove(cell->entities.begin(), cell->entities.end(),
						this),
					cell->entities.end());
				ownerCells.clear();
			}
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


typedef class NodeEntity : public Entity {
private:
	Entity* parent_entity = nullptr;

public:
	Cell* ownerCell = nullptr;

	NodeEntity(Manager& mManager) : Entity(mManager) {

	}
	~NodeEntity() {

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

	void imgui_print() override {
		glm::vec2 position = this->GetComponent<TransformComponent>().getPosition();  // Make sure Entity class has a getPosition method
		ImGui::Text("Position: (%.2f, %.2f)", position.x, position.y);
	}
	

} Node;