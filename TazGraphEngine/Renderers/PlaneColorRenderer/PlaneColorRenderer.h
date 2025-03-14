#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#include "../../Vertex.h"
#include "../../GLSLProgram.h"

#define RECT_OFFSET 6
#define TRIANGLE_OFFSET 3

enum class ColorGlyphSortType {
	NONE,
	FRONT_TO_BACK,
	BACK_TO_FRONT
};

class ColorRenderBatch {
public:
	ColorRenderBatch(GLuint Offset, GLuint NumVertices, glm::vec3 CenterPos) : offset(Offset),
		numVertices(NumVertices),
		centerPos(CenterPos)
	{

	}
	GLuint offset;
	GLuint numVertices;
	glm::vec3 centerPos;
};


class TriangleColorGlyph {

public:
	TriangleColorGlyph() {};
	TriangleColorGlyph(
		const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3,
		float Depth, const Color& color
	)
		{

		glm::vec2 rotatedV1 = glm::vec2(v1.x, v1.y);
		glm::vec2 rotatedV2 = glm::vec2(v2.x, v2.y);
		glm::vec2 rotatedV3 = glm::vec2(v3.x, v3.y);


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
	ColorGlyph(const glm::vec4& destRect, float Depth, const Color& color)
		{

		glm::vec3 atopLeft(destRect.x, destRect.y, Depth);
		glm::vec3 abottomLeft(destRect.x, destRect.y + destRect.w, Depth);
		glm::vec3 abottomRight(destRect.x + destRect.z, destRect.y + destRect.w, Depth);
		glm::vec3 atopRight(destRect.x + destRect.z, destRect.y, Depth);

		topLeft.color = color;
		topLeft.setPosition(atopLeft.x, atopLeft.y, atopLeft.z);

		bottomLeft.color = color;
		bottomLeft.setPosition(abottomLeft.x, abottomLeft.y, abottomLeft.z );

		bottomRight.color = color;
		bottomRight.setPosition(abottomRight.x, abottomRight.y, abottomRight.z );

		topRight.color = color;
		topRight.setPosition(atopRight.x, atopRight.y, atopRight.z );
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
	void initColorBoxBatch(size_t mSize);

	void drawTriangle(size_t v_index, const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3,
		float depth, const Color& color);

	void draw(size_t v_index, const glm::vec4& destRect, float depth, const Color& color);

	void drawBox(const glm::vec4& destRect, float depth, const Color& color);

	void renderBatch(GLSLProgram* glsl_program);

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

	std::vector<ColorGlyph*> _boxGlyphPointers;
	std::vector<ColorGlyph> _boxGlyphs;

	std::vector<ColorRenderBatch> _renderBatches;
};