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
		const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3,
		const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3,
		GLuint texture, float Depth, const Color& color, float angle = 0.f
	)
		: texture(texture),
		depth(Depth) {

		float centerX = (v2.x + v3.x) / 2.0f; // not varykentro, just the middle
		float centerY = (v1.y + v2.y) / 2.0f;

		// Convert angle from degrees to radians if necessary
		float radians = glm::radians(angle);

		// Lambda to calculate rotated positions
		auto rotatePoint = [&](float x, float y) -> glm::vec2 {
			float s = sin(radians);
			float c = cos(radians);

			float dx = x - centerX;
			float dy = y - centerY;
			return glm::vec2(
				centerX + dx * c - dy * s,
				centerY + dx * s + dy * c
			);
			};

		glm::vec2 rotatedV1 = rotatePoint(v1.x, v1.y);
		glm::vec2 rotatedV2 = rotatePoint(v2.x, v2.y);
		glm::vec2 rotatedV3 = rotatePoint(v3.x, v3.y);


		topLeft.color = color;
		topLeft.setPosition(rotatedV1.x, rotatedV1.y, depth);
		topLeft.setUV(uv1.x, uv1.y);

		bottomLeft.color = color;
		bottomLeft.setPosition(rotatedV2.x, rotatedV2.y, depth);
		bottomLeft.setUV(uv2.x, uv2.y);

		bottomRight.color = color;
		bottomRight.setPosition(rotatedV3.x, rotatedV3.y, depth);
		bottomRight.setUV(uv3.x, uv3.y);
	};

	GLuint texture;
	float depth;

	Vertex topLeft;
	Vertex bottomLeft;
	Vertex bottomRight;
};

class Glyph {

public:
	Glyph() {};
	Glyph(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float Depth, const Color& color, float angle = 0.f)
		: texture(texture),
		depth(Depth) {

		float centerX = destRect.x + destRect.z / 2.0f;
		float centerY = destRect.y + destRect.w / 2.0f;

		// Convert angle from degrees to radians if necessary
		float radians = glm::radians(angle);

		// Lambda to calculate rotated positions
		auto rotatePoint = [&](float x, float y) -> glm::vec2 {
			float dx = x - centerX;
			float dy = y - centerY;
			return glm::vec2(
				centerX + dx * cos(radians) - dy * sin(radians),
				centerY + dx * sin(radians) + dy * cos(radians)
			);
			};

		glm::vec2 rotatedTopLeft = rotatePoint(destRect.x, destRect.y );
		glm::vec2 rotatedBottomLeft = rotatePoint(destRect.x, destRect.y + destRect.w);
		glm::vec2 rotatedBottomRight = rotatePoint(destRect.x + destRect.z, destRect.y + destRect.w);
		glm::vec2 rotatedTopRight = rotatePoint(destRect.x + destRect.z, destRect.y );

		topLeft.color = color;
		topLeft.setPosition(rotatedTopLeft.x, rotatedTopLeft.y, depth);
		topLeft.setUV(uvRect.x, uvRect.y ); // Use bottom y for top

		bottomLeft.color = color;
		bottomLeft.setPosition(rotatedBottomLeft.x, rotatedBottomLeft.y, depth);
		bottomLeft.setUV(uvRect.x, uvRect.y + uvRect.w); // Use top y for bottom

		bottomRight.color = color;
		bottomRight.setPosition(rotatedBottomRight.x, rotatedBottomRight.y, depth);
		bottomRight.setUV(uvRect.x + uvRect.z, uvRect.y + uvRect.w); // Use top y for bottom

		topRight.color = color;
		topRight.setPosition(rotatedTopRight.x, rotatedTopRight.y, depth);
		topRight.setUV(uvRect.x + uvRect.z, uvRect.y ); // Use bottom y for top
	};

	GLuint texture;
	float depth;

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

	void begin(GlyphSortType sortType = GlyphSortType::TEXTURE);
	void end();

	void initBatch(size_t mSize);
	void drawTriangle(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3, const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3, GLuint texture, float depth, const Color& color, float angle);

	void draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const Color& color, float angle = 0);

	void renderBatch();

	void dispose();
private:
	void createRenderBatches();
	void createVertexArray();
	void sortGlyphs();

	static bool compareFrontToBack(Glyph* a, Glyph* b);
	static bool compareBackToFront(Glyph* a, Glyph* b);
	static bool compareTexture(Glyph* a, Glyph* b);

	GLuint _vbo;
	GLuint _vao;

	GlyphSortType _sortType;

	std::vector<Glyph*> _glyphPointers; //this is for sorting
	std::vector<Glyph> _glyphs; //actual glyphs

	std::vector<TriangleGlyph*> _triangleGlyphPointers; //this is for sorting
	std::vector<TriangleGlyph> _triangleGlyphs; //actual glyphs

	std::vector<RenderBatch> _renderBatches;
};