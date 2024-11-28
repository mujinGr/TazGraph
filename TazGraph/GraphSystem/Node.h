#pragma once
#include "GraphSystem.h"

class Node : public GraphObject 
{
public:

	Node() : _x(0), _y(0), _width(0), _height(0) {

	}

	Node(float x, float y, float width, float height) : _x(x), _y(y), _width(width), _height(height)  {

	}

	void init() override {
		GraphObject::init();

		float vertexData[12];

		// first triangle
		vertexData[0] = _x + _width;
		vertexData[1] = _y + _height;
		
		vertexData[2] = _x;
		vertexData[3] = _y + _height;

		vertexData[4] = _x;
		vertexData[5] = _y;

		// second triangle
		vertexData[6] = _x;
		vertexData[7] = _y;

		vertexData[8] = _x + _width;
		vertexData[9] = _y;

		vertexData[10] = _x + _width;
		vertexData[11] = _y + _height;

		glBindBuffer(GL_ARRAY_BUFFER, _vboID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void update() override {

	}

	void draw() override {
		glBindBuffer(GL_ARRAY_BUFFER, _vboID);

		glEnableVertexAttribArray(0);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, _vboID);

	}
private:
	float _x = 0.f, _y, _width, _height;
};

