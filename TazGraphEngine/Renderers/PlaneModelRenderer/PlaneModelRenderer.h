#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#include "../../Vertex.h"

#define RECT_OFFSET 6
#define TRIANGLE_OFFSET 3

enum class GlyphSortType {
	NONE,
	FRONT_TO_BACK,
	BACK_TO_FRONT,
	TEXTURE
};

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
		const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, // v1 top one, v2 bot left and v3 bot right
		const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3,
		GLuint texture, const Color& color
	)
		: texture(texture)
		{

		glm::vec3 rotatedV1 = glm::vec3(v1.x, v1.y, v1.z);
		glm::vec3 rotatedV2 = glm::vec3(v2.x, v2.y, v2.z);
		glm::vec3 rotatedV3 = glm::vec3(v3.x, v3.y, v3.z);

		topLeft.color = color;
		topLeft.setPosition(rotatedV1.x, rotatedV1.y, rotatedV1.z);
		topLeft.setUV(uv1.x, uv1.y);

		bottomLeft.color = color;
		bottomLeft.setPosition(rotatedV2.x, rotatedV2.y, rotatedV1.z);
		bottomLeft.setUV(uv2.x, uv2.y);

		bottomRight.color = color;
		bottomRight.setPosition(rotatedV3.x, rotatedV3.y, rotatedV1.z);
		bottomRight.setUV(uv3.x, uv3.y);
	};

	GLuint texture;

	Vertex topLeft;
	Vertex bottomLeft;
	Vertex bottomRight;
};

class Glyph {

public:
	Glyph() {};
	Glyph(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float Depth, const Color& color)
		: texture(texture) {


		glm::vec3 rotatedTopLeft = glm::vec3(destRect.x, destRect.y, Depth);
		glm::vec3 rotatedBottomLeft = glm::vec3(destRect.x, destRect.y + destRect.w, Depth);
		glm::vec3 rotatedBottomRight = glm::vec3(destRect.x + destRect.z, destRect.y + destRect.w, Depth);
		glm::vec3 rotatedTopRight = glm::vec3(destRect.x + destRect.z, destRect.y, Depth);

		topLeft.color = color;
		topLeft.setPosition(rotatedTopLeft.x, rotatedTopLeft.y, rotatedTopLeft.z);
		topLeft.setUV(uvRect.x, uvRect.y ); // Use bottom y for top

		bottomLeft.color = color;
		bottomLeft.setPosition(rotatedBottomLeft.x, rotatedBottomLeft.y, rotatedBottomLeft.z);
		bottomLeft.setUV(uvRect.x, uvRect.y + uvRect.w); // Use top y for bottom

		bottomRight.color = color;
		bottomRight.setPosition(rotatedBottomRight.x, rotatedBottomRight.y, rotatedBottomRight.z);
		bottomRight.setUV(uvRect.x + uvRect.z, uvRect.y + uvRect.w); // Use top y for bottom

		topRight.color = color;
		topRight.setPosition(rotatedTopRight.x, rotatedTopRight.y, rotatedTopRight.z);
		topRight.setUV(uvRect.x + uvRect.z, uvRect.y ); // Use bottom y for top
	};

	GLuint texture;

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

	void begin(GlyphSortType sortType = GlyphSortType::BACK_TO_FRONT);
	void end();

	void initTriangleBatch(size_t mSize);
	void initQuadBatch(size_t mSize);

	void drawTriangle(
		const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
		const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3,
		GLuint texture, const Color& color);

	void draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const Color& color);

	void renderBatch();

	void dispose();
private:
	void createRenderBatches();
	void createVertexArray();

	GLuint _vbo;
	GLuint _vao;

	GlyphSortType _sortType;

	std::vector<Glyph> _glyphs; //actual glyphs

	std::vector<TriangleGlyph> _triangles; //actual glyphs

	std::vector<RenderBatch> _renderBatches;
};