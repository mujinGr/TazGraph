#pragma once

#include <cstddef>

#include "GraphSystem.h"
#include "../Vertex.h"

class Node : public GraphObject 
{
public:

	Node() : _x(0), _y(0), _width(0), _height(0) {

	}

	Node(float x, float y, float width, float height) : _x(x), _y(y), _width(width), _height(height)  {

	}

	void init() override {
		GraphObject::init();

		Vertex vertexData[6];

		// first triangle
		vertexData[0].position.x = _x + _width;
		vertexData[0].position.y = _y + _height;
		
		vertexData[1].position.x = _x;
		vertexData[1].position.y = _y + _height;

		vertexData[2].position.x = _x;
		vertexData[2].position.y = _y;

		// second triangle
		vertexData[3].position.x = _x;
		vertexData[3].position.y = _y;

		vertexData[4].position.x = _x + _width;
		vertexData[4].position.y = _y;

		vertexData[5].position.x = _x + _width;
		vertexData[5].position.y = _y + _height;

		for (int i = 0; i < 6; i++) {
			vertexData[i].color = Color(255, 0, 255, 255);
		}

		vertexData[1].color = Color(0, 0, 255, 255);
		vertexData[4].color = Color(255, 0, 0, 255);


		glBindBuffer(GL_ARRAY_BUFFER, _vboID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void update() override {

	}

	void draw() override {
		glBindBuffer(GL_ARRAY_BUFFER, _vboID);

		glEnableVertexAttribArray(0);

		// This is the position attribute
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

		// This is the color attribute
		glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, color));

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(1);

		glDisableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, _vboID);

	}
private:
	float _x = 0.f, _y, _width, _height;
};

