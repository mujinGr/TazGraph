#pragma once

#include "../../GLSLProgram.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#include "../../Vertex.h"




class RenderLineBatch {
public:
	RenderLineBatch() {

	}
	RenderLineBatch(GLuint Offset, GLuint NumIndices) : offset(Offset),
		numIndices(NumIndices){

	}
	GLuint offset = 0;
	GLuint numIndices = 0;
};

class LineGlyph {
	
public:
	LineGlyph() {};
	LineGlyph(const glm::vec3& fromPosition, const glm::vec3& toPosition, const Color& srcColor, const Color& destColor)
		{

		fromV.color = srcColor;
		fromV.setPosition(fromPosition);

		toV.color = destColor;
		toV.setPosition(toPosition);
	};

	ColorVertex fromV;
	ColorVertex toV;
};

class SquareGlyph {
	//todo avoid rotating every point individually, takes a lot of time
	//todo instead rotate them from transform using a global angle(based on aimpos) that is added to all transforms
	//todo then rotation in glyph is just taking that transform
public:
	SquareGlyph() {};
	SquareGlyph(const glm::vec4& destRect, const Color& color, float angle, float mdepth)
		{
		glm::vec3 atopLeft(destRect.x, destRect.y, mdepth);
		glm::vec3 abottomLeft(destRect.x, destRect.y + destRect.w, mdepth);
		glm::vec3 abottomRight(destRect.x + destRect.z, destRect.y + destRect.w, mdepth);
		glm::vec3 atopRight(destRect.x + destRect.z, destRect.y, mdepth);

		topLeft.color = color;
		topLeft.setPosition(atopLeft);

		bottomLeft.color = color;
		bottomLeft.setPosition(abottomLeft);

		bottomRight.color = color;
		bottomRight.setPosition(abottomRight);

		topRight.color = color;
		topRight.setPosition(atopRight);

	};

	ColorVertex topLeft;
	ColorVertex bottomLeft;
	ColorVertex bottomRight;
	ColorVertex topRight;
};

class BoxGlyph {

public:
	BoxGlyph() {};
	BoxGlyph(const glm::vec3& origin, const glm::vec3& size, const Color& color, float angle)
	{

		glm::vec3 atopLeft(origin.x, origin.y, origin.z);
		glm::vec3 abottomLeft(origin.x, origin.y + size.y, origin.z);
		glm::vec3 abottomRight(origin.x + size.x, origin.y + size.y, origin.z);
		glm::vec3 atopRight(origin.x + size.x, origin.y, origin.z);

		a_topLeft.color = color;
		a_topLeft.setPosition(atopLeft);

		a_bottomLeft.color = color;
		a_bottomLeft.setPosition(abottomLeft);

		a_bottomRight.color = color;
		a_bottomRight.setPosition(abottomRight);

		a_topRight.color = color;
		a_topRight.setPosition(atopRight);

		glm::vec3 btopLeft(origin.x, origin.y, origin.z + size.z);
		glm::vec3 bbottomLeft(origin.x, origin.y + size.y, origin.z + size.z);
		glm::vec3 bbottomRight(origin.x + size.x, origin.y + size.y, origin.z + size.z);
		glm::vec3 btopRight(origin.x + size.x, origin.y, origin.z + size.z);

		b_topLeft.color = color;
		b_topLeft.setPosition(btopLeft);

		b_bottomLeft.color = color;
		b_bottomLeft.setPosition(bbottomLeft);

		b_bottomRight.color = color;
		b_bottomRight.setPosition(bbottomRight);

		b_topRight.color = color;
		b_topRight.setPosition(btopRight);

	};

	ColorVertex a_topLeft;
	ColorVertex a_bottomLeft;
	ColorVertex a_bottomRight;
	ColorVertex a_topRight;

	ColorVertex b_topLeft;
	ColorVertex b_bottomLeft;
	ColorVertex b_bottomRight;
	ColorVertex b_topRight;
};

class LineRenderer {
public:
	const char* VERT_SRC = R"(#version 400

in vec3 vertexPosition; //vec3 is array of 3 floats
in vec4 vertexColor;

uniform mat4 rotationMatrix;

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
	void begin();

	void end();

	void initBatchLines(size_t msize);

	void initBatchSquares(size_t msize);

	void initBatchBoxes(size_t msize);

	void drawLine(size_t v_index, const glm::vec3 srcPosition, const glm::vec3 destPosition, const Color& srcColor, const Color& destColor);
	void drawRectangle(size_t v_index, const glm::vec4& destRect, const Color& color, float angle, float zIndex =0.0f);
	void drawBox(size_t v_index, const glm::vec3& origin, const glm::vec3& size, const Color& color, float angle);
	void drawCircle(const glm::vec2& center, const Color& color, float radius);

	void initBatchSize();
	void renderBatch(float lineWidth);
	
	void dispose();

	int box_edgePairs[12][2] = {
			{0, 1}, {1, 2}, {2, 3}, {3, 0}, // Bottom face
			{4, 5}, {5, 6}, {6, 7}, {7, 4}, // Top face
			{0, 4}, {1, 5}, {2, 6}, {3, 7}  // Vertical edges
	};

private:
	void createRenderBatches();
	void createVertexArray();

	GLuint _vbo = 0, _vao = 0, _ibo = 0; //! ibo is what is going to store the integers for each 

	std::vector<ColorVertex> _vertices;
	std::vector<GLuint> _indices;

	size_t _lineGlyphs_size		= 0;
	size_t _squareGlyphs_size	= 0;
	size_t _boxGlyphs_size		= 0;

	size_t _rectangles_verticesOffset = 0;
	size_t _lines_verticesOffset = 0;
	size_t _boxes_verticesOffset= 0;

	size_t _rectangles_indicesOffset = 0;
	size_t _lines_indicesOffset = 0;
	size_t _boxes_indicesOffset = 0;

	std::vector<RenderLineBatch> _renderBatches;
};