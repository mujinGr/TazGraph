#pragma once

#include <GL/glew.h>
#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/glm.hpp>
#include <vector>

#include "../../Vertex.h"
#include "../../GLSLProgram.h"

#define RECT_OFFSET 6
#define TRIANGLE_OFFSET 3

class RenderBatch {
public:
	RenderBatch(GLuint Offset, GLuint NumVertices, glm::vec3 CenterPos, GLuint Texture) : offset(Offset),
		numVertices(NumVertices),
		centerPos(CenterPos),
		texture(Texture) {

	}
	GLuint offset;
	GLuint numVertices;
	glm::vec3 centerPos = glm::vec3(0);
	GLuint texture;
};


class TriangleGlyph {

public:
	TriangleGlyph() {};
	TriangleGlyph(
		const glm::vec3& mRotation,
		const glm::vec2& m_v1, const glm::vec2& m_v2, const glm::vec2& m_v3, // v1 top one, v2 bot left and v3 bot right
		const glm::vec2& m_uv1, const glm::vec2& m_uv2, const glm::vec2& m_uv3,
		GLuint texture, const Color& color
	)
		: texture(texture)
		{


		glm::vec3 positions[3] = {
	   {m_v1, 0.0f},
	   {m_v2, 0.0f},
	   {m_v3, 0.0f}
		};
		//todo rotation is going to be done on gpu side, so we need to pass different renderBatches 
		//todo that going to load the uniform
		topLeft.color = color;
		topLeft.setPosition(positions[0]);
		topLeft.rotation = mRotation;
		topLeft.setUV(m_uv1);

		bottomLeft.color = color;
		bottomLeft.setPosition(positions[1]);
		bottomLeft.rotation = mRotation;
		bottomLeft.setUV(m_uv2);

		bottomRight.color = color;
		bottomRight.setPosition(positions[2]);
		bottomRight.rotation = mRotation;
		bottomRight.setUV(m_uv3);
	};

	GLuint texture = 0;

	Vertex topLeft;
	Vertex bottomLeft;
	Vertex bottomRight;
};

class Glyph {

public:
	Glyph() {};
	Glyph(const glm::vec2& rectSize,
		const glm::vec3& mRotation,
		const glm::vec4& uvRect,
		GLuint texture, float Depth,
		const Color& color)
		: texture(texture) {

		float halfW = rectSize.x / 2.0f;
		float halfH = rectSize.y / 2.0f;

		Position positions[4] = {
	   {-halfW, -halfH, 0.0f},  // topLeft
	   {-halfW,  halfH, 0.0f},  // bottomLeft
	   { halfW,  halfH, 0.0f},  // bottomRight
	   { halfW, -halfH, 0.0f}   // topRight
		};

		glm::vec2 uv_topLeft = glm::vec2(uvRect.x, uvRect.y);
		glm::vec2 uv_bottomLeft = glm::vec2(uvRect.x, uvRect.y + uvRect.w);
		glm::vec2 uv_bottomRight = glm::vec2(uvRect.x + uvRect.z, uvRect.y + uvRect.w);
		glm::vec2 uv_topRight = glm::vec2(uvRect.x + uvRect.z, uvRect.y);
			
		topLeft.color = color;
		topLeft.setPosition(positions[0]);
		topLeft.rotation = mRotation;
		topLeft.setUV(uv_topLeft); // Use bottom y for top

		bottomLeft.color = color;
		bottomLeft.setPosition(positions[1]);
		bottomLeft.rotation = mRotation;
		bottomLeft.setUV(uv_bottomLeft); // Use top y for bottom

		bottomRight.color = color;
		bottomRight.setPosition(positions[2]);
		bottomRight.rotation = mRotation;
		bottomRight.setUV(uv_bottomRight); // Use top y for bottom

		topRight.color = color;
		topRight.setPosition(positions[3]);
		topRight.rotation = mRotation;
		topRight.setUV(uv_topRight); // Use bottom y for top
	};

	GLuint texture = 0;

	Vertex topLeft;
	Vertex bottomLeft;
	Vertex topRight;
	Vertex bottomRight;
};

// init --_
//		 `-->begin() 
//		 |
//		 | --> draw()
//		 | --> draw()
//		 |
//		 |--> end() 
//		 `--> renderBatch() 

class PlaneModelRenderer {
public:
	PlaneModelRenderer();
	~PlaneModelRenderer();

	void init();

	void begin();
	void end();

	void initTextureTriangleBatch(size_t mSize);
	void initTextureQuadBatch(size_t mSize);

	void initBatchSize();

	void drawTriangle(
		size_t v_index,
		const glm::vec3& triangleOffset,
		const glm::vec3& mRotation,
		const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3,
		GLuint texture, const Color& color);

	void draw(size_t v_index,
		const glm::vec3& rectOffset,
		const glm::vec2& rectSize,
		const glm::vec3& mRotation,
		const glm::vec4& uvRect,
		GLuint texture,
		const glm::vec3& bodyCenter, const Color& color);

	void renderBatch(GLSLProgram* glsl_program);

	void dispose();
private:
	void createRenderBatches();
	void createVertexArray();

	GLuint _vbo;
	GLuint _vao;

	std::vector<Vertex> _vertices;

	size_t _glyphs_size = 0; //actual glyphs
	size_t _triangles_size = 0; //actual glyphs

	int _triangles_verticesOffset = 0;
	int _rectangles_verticesOffset = 0;

	std::vector<RenderBatch> _renderBatches;
};
