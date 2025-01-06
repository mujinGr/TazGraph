#pragma once

#include "../../Components.h"

class LinkComponent : public Component
{
private:
	Entity* from = nullptr;
	Entity* to = nullptr;

public:

	LinkComponent() {

	}

	~LinkComponent() {

	}

	void init() override {
		
	}

	void update(float deltaTime) override {
		
	}

	void draw(PlaneModelRenderer& batch, TazGraphEngine::Window& window) override {
			
	}
};