#include "DebugRenderer.h"

constexpr float PI = 3.14159265358f;

namespace {
	const char* VERT_SRC = R"(#version 400

in vec3 vertexPosition; //vec3 is array of 3 floats
in vec4 vertexColor;
in vec2 vertexUV;

out vec4 fragmentColor;
out vec2 fragmentUV;

uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(vertexPosition.xyz, 1.0);

    fragmentColor = vertexColor;

    fragmentUV = vertexUV;
})";

	const char* FRAG_SRC = R"(#version 400

in vec4 fragmentColor;
in vec2 fragmentUV;

out vec4 color; //rgb value

void main() {
    color = vec4(fragmentColor.rgb, fragmentColor.a);
})";
}


DebugRenderer::DebugRenderer()
{
}

DebugRenderer::~DebugRenderer()
{
	dispose();
}

void DebugRenderer::init()
{
	// Shader init
	_program.compileShadersFromSource(VERT_SRC, FRAG_SRC);
	_program.addAttribute("vertexPosition");
	_program.addAttribute("vertexColor");
	_program.linkShaders();

	//Set up buffers
	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
	glGenBuffers(1, &_ibo);

	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo); //Buffer used for index drawing

	glEnableVertexAttribArray(0);
	// 0 attrib, 2 floats, type, not normalised, sizeof Vertex, pointer to the Vertex Data
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (void *)offsetof(DebugVertex, position)); 
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(DebugVertex), (void*)offsetof(DebugVertex, color));

	glBindVertexArray(0); //! the buffers are bound withing the context of vao so when that is
								//! unbinded all the other vbos are also unbinded
}

void DebugRenderer::end()
{
	//Bind buffer for the information for the vertexes
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	//Orphan the buffer : replace the memory block for the buffer object, 
	//dont wait the gpu for operations on the old data
	glBufferData(GL_ARRAY_BUFFER, _verts.size() * sizeof(DebugVertex), nullptr, GL_DYNAMIC_DRAW);
	//Upload the data
	glBufferSubData(GL_ARRAY_BUFFER, 0, _verts.size() * sizeof(DebugVertex), _verts.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Bind buffer for the indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
	//Orphan the buffer : replace the memory block for the buffer object, 
	//dont wait the gpu for operations on the old data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
	//Upload the data
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, _indices.size() * sizeof(GLuint), _indices.data());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	_numElements = _indices.size();
	_indices.clear();
	_verts.clear();
}

void DebugRenderer::drawLine(const glm::vec2 srcPosition, const glm::vec2 destPosition, const Color& color, float zIndex)
{
	int i = _verts.size();
	_verts.resize(_verts.size() + 2); // more efficient than calling pushBack 4 times

	auto linkPoint = [&](float x, float y) -> glm::vec3 {
		return glm::vec3(x,y,zIndex);
		};

	_verts[i].setPosition(linkPoint(srcPosition.x, srcPosition.y ));
	_verts[i + 1].setPosition(linkPoint(destPosition.x, destPosition.y));

	for (int j = i; j < i + 2; j++) {
		_verts[j].color = color;
	}

	_indices.reserve(_indices.size() + 2); // indices for the ibo

	_indices.push_back(i);
	_indices.push_back(i + 1);

}

void DebugRenderer::drawBox(const glm::vec4& destRect, const Color& color, float angle, float zIndex)
{
	int i = _verts.size();
	_verts.resize(_verts.size() + 4); // more efficient than calling pushBack 4 times

	float centerX = destRect.x + destRect.z / 2.0f;
	float centerY = destRect.y + destRect.w / 2.0f;

	// Convert angle from degrees to radians if necessary
	float radians = glm::radians(angle);

	// Lambda to calculate rotated positions
	auto rotatePoint = [&](float x, float y) -> glm::vec3 {
		float dx = x - centerX;
		float dy = y - centerY;
		return glm::vec3(
			centerX + dx * cos(radians) - dy * sin(radians),
			centerY + dx * sin(radians) + dy * cos(radians),
			zIndex
		);
		};

	glm::vec2 positionOffset(destRect.x , destRect.y );

	_verts[  i  ].setPosition( rotatePoint(positionOffset.x, positionOffset.y + destRect.w));
	_verts[i + 1].setPosition(rotatePoint(positionOffset.x, positionOffset.y));
	_verts[i + 2].setPosition(rotatePoint(positionOffset.x + destRect.z, positionOffset.y));
	_verts[i + 3].setPosition(rotatePoint(positionOffset.x + destRect.z, positionOffset.y + destRect.w));

	for (int j = i; j < i + 4; j++) {
		_verts[j].color = color;
	}

	_indices.reserve(_indices.size() + 8); // indices for the ibo // + the number of verts in total

	_indices.push_back(  i  );
	_indices.push_back(i + 1);

	_indices.push_back(i + 1);
	_indices.push_back(i + 2);

	_indices.push_back(i + 2);
	_indices.push_back(i + 3);

	_indices.push_back(i + 3);
	_indices.push_back(  i  );

}

void DebugRenderer::drawCircle(const glm::vec2& center, const Color& color, float radius)
{
	static const int NUM_VERTS = 255;
	// Set up vertices
	int start = _verts.size();
	_verts.resize(_verts.size() + NUM_VERTS);
	for (int i = 0; i < NUM_VERTS; i++) {
		float angle = ((float)i / NUM_VERTS) * 2.0f * PI;

		_verts[start + i].position.x = cos(angle) * radius + center.x;
		_verts[start + i].position.y = sin(angle) * radius + center.y;
		_verts[start + i].color = color;
	}

	// Set up indexes for drawing
	_indices.reserve(_indices.size() + NUM_VERTS * 2);

	for (int i = 0; i < NUM_VERTS - 1; i++) {
		_indices.push_back(start + i);
		_indices.push_back(start + i + 1);
	}
	_indices.push_back(start + NUM_VERTS - 1);
	_indices.push_back(start);
}

void DebugRenderer::render( const glm::mat4& projectionMatrix, float lineWidth )
{
	_program.use();

	GLint pLocation = _program.getUniformLocation("projection");
	glUniformMatrix4fv(pLocation, 1, GL_FALSE, &(projectionMatrix[0][0]));

	glLineWidth(lineWidth);
	glBindVertexArray(_vao);
	glDrawElements(GL_LINES, _numElements, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	_program.unuse();
}

void DebugRenderer::dispose()
{
	if (_vao) {
		glDeleteVertexArrays(1, &_vao);
	}
	if (_vbo) {
		glDeleteBuffers(1, &_vbo);
	}
	if (_ibo) {
		glDeleteBuffers(1, &_ibo);
	}
	_program.dispose();
}
