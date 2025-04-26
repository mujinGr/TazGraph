#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#include "../../Vertex.h"
#include "../../GLSLProgram.h"

#define RECT_OFFSET 6
#define TRIANGLE_OFFSET 3

#define SQUARE_OFFSET 4


class ColorRenderBatchElements {
public:
	ColorRenderBatchElements() {

	}
	ColorRenderBatchElements(GLuint Offset, GLuint NumIndices, glm::vec3 CenterPos) : offset(Offset),
		numIndices(NumIndices),
		centerPos(CenterPos)
	{

	}
	GLuint offset = 0;
	GLuint numIndices = 0;
	glm::vec3 centerPos = glm::vec3(0);
};

class ColorRenderBatchArrays {
public:
	ColorRenderBatchArrays() {}
	ColorRenderBatchArrays(GLuint Offset, GLuint NumVertices, glm::vec3 CenterPos) : offset(Offset),
		numVertices(NumVertices),
		centerPos(CenterPos)
	{

	}
	GLuint offset = 0;
	GLuint numVertices = 0;
	glm::vec3 centerPos = glm::vec3(0);
};


class TriangleColorGlyph {

public:
	TriangleColorGlyph() {};
	TriangleColorGlyph(
		const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3, 
		const glm::vec3& cpuRotation/*? THIS SHOULD NOT BE LIKE THIS, ROTATIONS MANAGED ON GPU*/, const Color& color
	)
		{

		glm::vec3 positions[3] = {
	   {v1, 0.0f},  
	   {v2, 0.0f},  
	   {v3, 0.0f}
		};


		topLeft.color = color;
		topLeft.setPosition(glm::vec3(rotatePoint(positions[0].x, positions[0].y, cpuRotation.z), positions[0].z));

		bottomLeft.color = color;
		bottomLeft.setPosition(glm::vec3(rotatePoint(positions[1].x, positions[1].y, cpuRotation.z), positions[1].z));

		bottomRight.color = color;
		bottomRight.setPosition(glm::vec3(rotatePoint(positions[2].x, positions[2].y, cpuRotation.z), positions[2].z));
	};

	ColorVertex topLeft;
	ColorVertex bottomLeft;
	ColorVertex bottomRight;
};

class BoxColorGlyph {

public:
	// Lambda to calculate rotated positions

	BoxColorGlyph() {};
	BoxColorGlyph(const glm::vec3& boxSize, const Color& color)
	{
		float halfW = boxSize.x / 2.0f;
		float halfH = boxSize.y / 2.0f;
		float halfZ = boxSize.z / 2.0f;

		Position positions[8] = {
	   {-halfW, -halfH, -halfZ},
	   {-halfW,  halfH, -halfZ},
	   { halfW,  halfH, -halfZ},
	   { halfW, -halfH, -halfZ},
	   { -halfW, -halfH, halfZ},
	   {-halfW,  halfH, halfZ},
	   { halfW,  halfH, halfZ},
	   { halfW, -halfH, halfZ}
		};

		a_topLeft.color = color;
		a_topLeft.setPosition(positions[0]);

		a_bottomLeft.color = color;
		a_bottomLeft.setPosition(positions[1]);

		a_bottomRight.color = color;
		a_bottomRight.setPosition(positions[2]);

		a_topRight.color = color;
		a_topRight.setPosition(positions[3]);

		b_topLeft.color = color;
		b_topLeft.setPosition(positions[4]);

		b_bottomLeft.color = color;
		b_bottomLeft.setPosition(positions[5]);

		b_bottomRight.color = color;
		b_bottomRight.setPosition(positions[6]);

		b_topRight.color = color;
		b_topRight.setPosition(positions[7]);
	};

	ColorVertex a_topLeft;
	ColorVertex a_bottomLeft;
	ColorVertex a_topRight;
	ColorVertex a_bottomRight;
	ColorVertex b_topLeft;
	ColorVertex b_bottomLeft;
	ColorVertex b_topRight;
	ColorVertex b_bottomRight;
};

class ColorGlyph {

public:
	// Lambda to calculate rotated positions
	
	ColorGlyph() {};
	ColorGlyph(const glm::vec2& rectSize, const Color& color)
		{
		float halfW = rectSize.x / 2.0f;
		float halfH = rectSize.y / 2.0f;

		Position positions[4] = {
	   {-halfW, -halfH, 0.0f},  // topLeft
	   {-halfW,  halfH, 0.0f},  // bottomLeft
	   { halfW,  halfH, 0.0f},  // bottomRight
	   { halfW, -halfH, 0.0f}   // topRight
		};

		topLeft.color = color;
		topLeft.setPosition(positions[0]);

		bottomLeft.color = color;
		bottomLeft.setPosition(positions[1]);

		bottomRight.color = color;
		bottomRight.setPosition(positions[2]);

		topRight.color = color;
		topRight.setPosition(positions[3]);
	};

	ColorVertex topLeft;
	ColorVertex bottomLeft;
	ColorVertex topRight;
	ColorVertex bottomRight;
};

// init --_
//		 `-->begin() 
//		 |
//		 | --> draw()
//		 | --> draw()
//		 |
//		 |--> end() 
//		 `--> renderBatch() 

class PlaneColorRenderer {
public:
	PlaneColorRenderer();
	~PlaneColorRenderer();

	void init();

	void begin();
	void end();

	void initColorTriangleBatch(size_t mSize);
	void initColorQuadBatch(size_t mSize);
	void initBatchSize();
	void initColorBoxBatch(size_t mSize);

	void drawTriangle(size_t v_index, const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3,
		const glm::vec3& depth,
		const glm::vec3& cpuRotation, const Color& color);

	void draw(size_t v_index, const glm::vec2& rectSize, const glm::vec3& bodyCenter, const Color& color);

	void drawBox(const glm::vec4& destRect, float depth, const Color& color);

	void renderBatch(GLSLProgram* glsl_program);

	void dispose();

	int rect_triangleIndices[2][3] = {
			{0, 1, 2},
			{2, 3, 0}
	};

private:
	void createRenderBatches();
	void createVertexArray();

	GLuint _ibo;
	GLuint _vbo;
	GLuint _vao;

	std::vector<GLuint> _indices;
	std::vector<ColorVertex> _vertices; //actual glyphs

	size_t _glyphs_size = 0; //actual glyphs
	size_t _triangleGlyphs_size = 0; //actual glyphs
	size_t _boxGlyphs_size = 0;

	int _triangles_verticesOffset = 0;
	int _rectangles_verticesOffset = 0;

	std::vector<ColorRenderBatchArrays> _renderBatchesArrays;
	std::vector<ColorRenderBatchElements> _renderBatchesElements;

};