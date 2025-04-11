#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#include "../../Vertex.h"
#include "../../GLSLProgram.h"

#define RECT_OFFSET 6
#define TRIANGLE_OFFSET 3

class ColorRenderBatch {
public:
	ColorRenderBatch() {}
	ColorRenderBatch(GLuint Offset, GLuint NumVertices, glm::vec3 CenterPos) : offset(Offset),
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
		const glm::vec3& cpuRotation, const Color& color
	)
		{

		glm::vec3 positions[3] = {
	   {v1, 0.0f},  
	   {v2, 0.0f},  
	   {v3, 0.0f}
		};


		topLeft.color = color;
		topLeft.position = glm::vec3(rotatePoint(positions[0].x, positions[0].y, cpuRotation.z), positions[0].z);

		bottomLeft.color = color;
		bottomLeft.position = glm::vec3(rotatePoint(positions[1].x, positions[1].y, cpuRotation.z), positions[1].z);

		bottomRight.color = color;
		bottomRight.position = glm::vec3(rotatePoint(positions[2].x, positions[2].y, cpuRotation.z), positions[2].z);;
	};

	ColorVertex topLeft;
	ColorVertex bottomLeft;
	ColorVertex bottomRight;
};

class BoxColorGlyph {

public:
	// Lambda to calculate rotated positions

	BoxColorGlyph() {};
	BoxColorGlyph(const glm::vec4& destRect, float Depth, const Color& color)
	{

		glm::vec3 atopLeft(destRect.x, destRect.y, Depth);
		glm::vec3 abottomLeft(destRect.x, destRect.y + destRect.w, Depth);
		glm::vec3 abottomRight(destRect.x + destRect.z, destRect.y + destRect.w, Depth);
		glm::vec3 atopRight(destRect.x + destRect.z, destRect.y, Depth);

		topLeft.color = color;
		topLeft.setPosition(atopLeft.x, atopLeft.y, atopLeft.z);

		bottomLeft.color = color;
		bottomLeft.setPosition(abottomLeft.x, abottomLeft.y, abottomLeft.z);

		bottomRight.color = color;
		bottomRight.setPosition(abottomRight.x, abottomRight.y, abottomRight.z);

		topRight.color = color;
		topRight.setPosition(atopRight.x, atopRight.y, atopRight.z);
	};

	ColorVertex topLeft;
	ColorVertex bottomLeft;
	ColorVertex topRight;
	ColorVertex bottomRight;
};

class ColorGlyph {

public:
	// Lambda to calculate rotated positions
	
	ColorGlyph() {};
	ColorGlyph(const glm::vec2& rectSize, const Color& color)
		{
		float halfW = rectSize.x * 0.5f;
		float halfH = rectSize.y * 0.5f;

		glm::vec3 positions[4] = {
	   {-halfW, -halfH, 0.0f},  // topLeft
	   {-halfW,  halfH, 0.0f},  // bottomLeft
	   { halfW,  halfH, 0.0f},  // bottomRight
	   { halfW, -halfH, 0.0f}   // topRight
		};

		topLeft.color = color;
		topLeft.position = positions[0];

		bottomLeft.color = color;
		bottomLeft.position = positions[1];

		bottomRight.color = color;
		bottomRight.position = positions[2];

		topRight.color = color;
		topRight.position = positions[3];
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
private:
	void createRenderBatches();
	void createVertexArray();

	GLuint _vbo;
	GLuint _vao;

	std::vector<ColorVertex> _vertices; //actual glyphs

	size_t _glyphs_size = 0; //actual glyphs
	size_t _triangleGlyphs_size = 0; //actual glyphs
	size_t _boxGlyphs_size = 0;

	int _triangles_verticesOffset = 0;
	int _rectangles_verticesOffset = 0;

	std::vector<ColorRenderBatch> _renderBatches;
};