#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#include "../../Vertex.h"

#define RECT_OFFSET 6
#define TRIANGLE_OFFSET 3

enum class ColorGlyphSortType {
	NONE,
	FRONT_TO_BACK,
	BACK_TO_FRONT
};

class ColorRenderBatch {
public:
	ColorRenderBatch(GLuint Offset, GLuint NumVertices) : offset(Offset),
		numVertices(NumVertices) {

	}
	GLuint offset;
	GLuint numVertices;
};


class TriangleColorGlyph {

public:
	TriangleColorGlyph() {};
	TriangleColorGlyph(
		const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3,
		float Depth, const Color& color, float angle = 0.f
	)
		{

		float centerX = (v2.x + v3.x) / 2.0f; // not varykentro, just the middle
		float centerY = (v1.y + v2.y) / 2.0f;

		// Convert angle from degrees to radians if necessary
		float radians = glm::radians(angle);

		glm::vec2 rotatedV1 = rotatePoint(v1.x, v1.y, centerX, centerY, radians);
		glm::vec2 rotatedV2 = rotatePoint(v2.x, v2.y, centerX, centerY, radians);
		glm::vec2 rotatedV3 = rotatePoint(v3.x, v3.y, centerX, centerY, radians);


		topLeft.color = color;
		topLeft.setPosition(rotatedV1.x, rotatedV1.y, Depth);

		bottomLeft.color = color;
		bottomLeft.setPosition(rotatedV2.x, rotatedV2.y, Depth);

		bottomRight.color = color;
		bottomRight.setPosition(rotatedV3.x, rotatedV3.y, Depth);
	};

	ColorVertex topLeft;
	ColorVertex bottomLeft;
	ColorVertex bottomRight;
};

class ColorGlyph {

public:
	// Lambda to calculate rotated positions
	
	ColorGlyph() {};
	ColorGlyph(const glm::vec4& destRect, float Depth, const Color& color, float angle = 0.f)
		{

		float centerX = destRect.x + destRect.z / 2.0f;
		float centerY = destRect.y + destRect.w / 2.0f;
		float centerZ = Depth;

		// Convert angle from degrees to radians if necessary
		float radians = glm::radians(angle);


		glm::vec3 atopLeft(destRect.x, destRect.y, Depth);
		glm::vec3 abottomLeft(destRect.x, destRect.y + destRect.w, Depth);
		glm::vec3 abottomRight(destRect.x + destRect.z, destRect.y + destRect.w, Depth);
		glm::vec3 atopRight(destRect.x + destRect.z, destRect.y, Depth);

		glm::vec3 rotatedTopLeft = rotatePoint(atopLeft.x, atopLeft.y, atopLeft.z, centerX, centerY, centerZ, angle, 0, 0);
		glm::vec3 rotatedBottomLeft = rotatePoint(abottomLeft.x, abottomLeft.y, abottomLeft.z, centerX, centerY, centerZ, angle, 0, 0);
		glm::vec3 rotatedBottomRight = rotatePoint(abottomRight.x, abottomRight.y, abottomRight.z, centerX, centerY, centerZ, angle, 0, 0);
		glm::vec3 rotatedTopRight = rotatePoint(atopRight.x, atopRight.y, atopRight.z, centerX, centerY, centerZ, angle, 0, 0);

		topLeft.color = color;
		topLeft.setPosition(rotatedTopLeft.x, rotatedTopLeft.y, rotatedTopLeft.z);

		bottomLeft.color = color;
		bottomLeft.setPosition(rotatedBottomLeft.x, rotatedBottomLeft.y, rotatedBottomLeft.z );

		bottomRight.color = color;
		bottomRight.setPosition(rotatedBottomRight.x, rotatedBottomRight.y, rotatedBottomRight.z );

		topRight.color = color;
		topRight.setPosition(rotatedTopRight.x, rotatedTopRight.y, rotatedTopRight.z );
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

	void begin(ColorGlyphSortType  sortType = ColorGlyphSortType::BACK_TO_FRONT);
	void end();

	void initColorTriangleBatch(size_t mSize);
	void initColorQuadBatch(size_t mSize);

	void drawTriangle(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3,
		float depth, const Color& color, float angle);

	void draw(const glm::vec4& destRect, float depth, const Color& color, float angle = 0);

	void renderBatch();

	void dispose();
private:
	void createRenderBatches();
	void createVertexArray();
	void sortGlyphs();

	static bool compareFrontToBack(ColorGlyph* a, ColorGlyph* b);
	static bool compareBackToFront(ColorGlyph* a, ColorGlyph* b);

	GLuint _vbo;
	GLuint _vao;

	ColorGlyphSortType  _sortType;

	std::vector<ColorGlyph*> _glyphPointers; //this is for sorting
	std::vector<ColorGlyph> _glyphs; //actual glyphs

	std::vector<TriangleColorGlyph*> _triangleGlyphPointers; //this is for sorting
	std::vector<TriangleColorGlyph> _triangleGlyphs; //actual glyphs

	std::vector<ColorRenderBatch> _renderBatches;
};