#pragma once

#include <GL/glew.h>
#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/glm.hpp>
#include <vector>

#include "../../Vertex.h"

#define RECT_OFFSET 6
#define TRIANGLE_OFFSET 3

class RenderBatch {
public:
	RenderBatch(GLuint Offset, GLuint NumVertices, GLuint Texture) : offset(Offset),
		numVertices(NumVertices), texture(Texture) {

	}
	GLuint offset;
	GLuint numVertices;
	GLuint texture;
};


class TriangleGlyph {

public:
	TriangleGlyph() {};
	TriangleGlyph(
		const glm::vec3& m_v1, const glm::vec3& m_v2, const glm::vec3& m_v3, // v1 top one, v2 bot left and v3 bot right
		const glm::vec2& m_uv1, const glm::vec2& m_uv2, const glm::vec2& m_uv3,
		GLuint texture, const Color& color
	)
		: texture(texture)
		{
		//todo rotation is going to be done on gpu side, so we need to pass different renderBatches 
		//todo that going to load the uniform
		glm::vec3 v1 = glm::vec3(m_v1.x, m_v1.y, m_v1.z);
		glm::vec3 v2 = glm::vec3(m_v2.x, m_v2.y, m_v2.z);
		glm::vec3 v3 = glm::vec3(m_v3.x, m_v3.y, m_v3.z);

		topLeft.color = color;
		topLeft.setPosition(v1.x, v1.y, v1.z);
		topLeft.setUV(m_uv1.x, m_uv1.y);

		bottomLeft.color = color;
		bottomLeft.setPosition(v2.x, v2.y, v2.z);
		bottomLeft.setUV(m_uv2.x, m_uv2.y);

		bottomRight.color = color;
		bottomRight.setPosition(v3.x, v3.y, v3.z);
		bottomRight.setUV(m_uv3.x, m_uv3.y);
	};

	GLuint texture = 0;

	Vertex topLeft;
	Vertex bottomLeft;
	Vertex bottomRight;
};

class Glyph {

public:
	Glyph() {};
	Glyph(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float Depth, const Color& color)
		: texture(texture) {


		glm::vec3 t_topLeft = glm::vec3(destRect.x, destRect.y, Depth);
		glm::vec3 t_bottomLeft = glm::vec3(destRect.x, destRect.y + destRect.w, Depth);
		glm::vec3 t_bottomRight = glm::vec3(destRect.x + destRect.z, destRect.y + destRect.w, Depth);
		glm::vec3 t_topRight = glm::vec3(destRect.x + destRect.z, destRect.y, Depth);

		topLeft.color = color;
		topLeft.setPosition(t_topLeft.x, t_topLeft.y, t_topLeft.z);
		topLeft.setUV(uvRect.x, uvRect.y ); // Use bottom y for top

		bottomLeft.color = color;
		bottomLeft.setPosition(t_bottomLeft.x, t_bottomLeft.y, t_bottomLeft.z);
		bottomLeft.setUV(uvRect.x, uvRect.y + uvRect.w); // Use top y for bottom

		bottomRight.color = color;
		bottomRight.setPosition(t_bottomRight.x, t_bottomRight.y, t_bottomRight.z);
		bottomRight.setUV(uvRect.x + uvRect.z, uvRect.y + uvRect.w); // Use top y for bottom

		topRight.color = color;
		topRight.setPosition(t_topRight.x, t_topRight.y, t_topRight.z);
		topRight.setUV(uvRect.x + uvRect.z, uvRect.y ); // Use bottom y for top
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
		const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
		const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3,
		GLuint texture, const Color& color);

	void draw(size_t v_index, const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const Color& color);

	void renderBatch();

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
