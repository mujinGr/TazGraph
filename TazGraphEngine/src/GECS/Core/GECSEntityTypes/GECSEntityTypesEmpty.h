#pragma once

#include "./GECSEntityTypes.h"

class Empty : public EmptyEntity {

public:

	Empty(Manager& mManager) : EmptyEntity(mManager) {

	}

	void addGroup(Group mGroup) override {
		Entity::addGroup(mGroup);
		manager.AddToGroup(this, mGroup);
	}

	void removeGroup(Group mGroup) override {
		Entity::removeGroup(mGroup);
		manager.aboutTo_updateActiveEntities();
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
		manager.aboutTo_updateActiveEntities(); //? cant have it at destroy in baseclass
		// may need to also update Visible Entities
	}
};