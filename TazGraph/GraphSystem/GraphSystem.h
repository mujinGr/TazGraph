#pragma once
#include <GL/glew.h>
class GraphObject {
public:
	GraphObject() {
		_vboID = 0;
	}
	~GraphObject() {
		if (_vboID != 0) {
			glDeleteBuffers(1, &_vboID);
		}
	}

	virtual void init() {
		if (_vboID == 0) {
			glGenBuffers(1, &_vboID);
		}
	}

	virtual void update() {}

	virtual void draw() {}
protected:
	GLuint _vboID;
};