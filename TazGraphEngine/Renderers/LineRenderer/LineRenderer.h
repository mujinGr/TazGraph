#pragma once

#include "../../GLSLProgram.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../../Vertex.h"
#include "../../Camera2.5D/CameraManager.h"
#include <vector>


class LineRenderer {
public:
	const char* VERT_SRC = R"(#version 400

in vec3 vertexPosition; //vec3 is array of 3 floats
in vec4 vertexColor;

out vec4 fragmentColor;

uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(vertexPosition.xyz, 1.0);

    fragmentColor = vertexColor;
})";

	const char* FRAG_SRC = R"(#version 400

in vec4 fragmentColor;

out vec4 color; //rgb value

void main() {
    color = vec4(fragmentColor.rgb, fragmentColor.a);
})";

	LineRenderer();
	~LineRenderer();

	void init();
	void end();
	void drawLine(const glm::vec2 srcPosition, const glm::vec2 destPosition, const Color& srcColor, const Color& destColor, float zIndex);
	void drawBox(const glm::vec4& destRect, const Color& color, float angle, float zIndex =0.0f);
	void drawCircle(const glm::vec2& center, const Color& color, float radius);
	void renderBatch(float lineWidth);
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
	std::vector<LineVertex> _verts;
	std::vector<GLuint> _indices;
	GLuint _vbo = 0, _vao = 0, _ibo = 0; //! ibo is what is going to store the integers for each 
	//! vertex so we can use index drawing, without specifying the lines by duplicating the vertices
	int _numElements = 0;
};