#pragma once

#include "../../GLSLProgram.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../../Vertex.h"
#include "../../Camera2.5D/CameraManager.h"
#include <vector>


class LineRenderer {
public:
	LineRenderer();
	~LineRenderer();

	void init();
	void end();
	void drawLine(const glm::vec2 srcPosition, const glm::vec2 destPosition, const Color& color, float zIndex);
	void drawBox(const glm::vec4& destRect, const Color& color, float angle, float zIndex =0.0f);
	void drawCircle(const glm::vec2& center, const Color& color, float radius);
	void renderBatch(const glm::mat4& projectionMatrix, float lineWidth);
	void dispose();

	struct LineVertex { //instead of using the general Vertex that has also info about texture
							// we use this where we want just color
		Position position;
		Color color;

		void setPosition(glm::vec3 mposition) {
			position.x = mposition.x;
			position.y = mposition.y;
			position.z = mposition.z;
		}
	};

private:
	GLSLProgram _program;
	std::vector<LineVertex> _verts;
	std::vector<GLuint> _indices;
	GLuint _vbo = 0, _vao = 0, _ibo = 0; //! ibo is what is going to store the integers for each 
	//! vertex so we can use index drawing, without specifying the lines by duplicating the vertices
	int _numElements = 0;
};