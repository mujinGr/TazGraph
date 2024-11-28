#pragma once
#include "GraphSystem.h"

class Link : public GraphObject
{
	Link() {

	}

	Link(float srcX, float srcY, float destX, float destY) : _srcX(srcX), _srcY(srcY), _destX(destX), _destY(destY) {

	}

	void init() override {

	}

	void update() override {

	}

	void draw() override {

	}
private: 
	float _srcX, _srcY, _destX, _destY;
};
