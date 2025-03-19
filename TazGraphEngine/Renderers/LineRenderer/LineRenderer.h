#pragma once

#include "../../GLSLProgram.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#include "../../Vertex.h"

#define LINE_OFFSET 2
#define SQUARE_OFFSET 4
#define BOX_OFFSET 8


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
		fromV.setPosition(fromPosition.x, fromPosition.y, fromPosition.z);

		toV.color = destColor;
		toV.setPosition(toPosition.x, toPosition.y, toPosition.z);
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

		float centerX = destRect.x + destRect.z / 2.0f;
		float centerY = destRect.y + destRect.w / 2.0f;
		float centerZ = mdepth;

		// Convert angle from degrees to radians if necessary
		float radians = glm::radians(angle);

		glm::vec3 atopLeft(destRect.x, destRect.y, mdepth);
		glm::vec3 abottomLeft(destRect.x, destRect.y + destRect.w, mdepth);
		glm::vec3 abottomRight(destRect.x + destRect.z, destRect.y + destRect.w, mdepth);
		glm::vec3 atopRight(destRect.x + destRect.z, destRect.y, mdepth);

		glm::vec3 rotatedTopLeft = glm::vec3(atopLeft.x, atopLeft.y, atopLeft.z);
		glm::vec3 rotatedBottomLeft = glm::vec3(abottomLeft.x, abottomLeft.y, abottomLeft.z);
		glm::vec3 rotatedBottomRight = glm::vec3(abottomRight.x, abottomRight.y, abottomRight.z);
		glm::vec3 rotatedTopRight = glm::vec3(atopRight.x, atopRight.y, atopRight.z);

		topLeft.color = color;
		topLeft.setPosition(rotatedTopLeft.x, rotatedTopLeft.y, rotatedTopLeft.z);

		bottomLeft.color = color;
		bottomLeft.setPosition(rotatedBottomLeft.x, rotatedBottomLeft.y, rotatedBottomLeft.z);

		bottomRight.color = color;
		bottomRight.setPosition(rotatedBottomRight.x, rotatedBottomRight.y, rotatedBottomRight.z);

		topRight.color = color;
		topRight.setPosition(rotatedTopRight.x, rotatedTopRight.y, rotatedTopRight.z);

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

		float centerX = origin.x + size.x / 2.0f;
		float centerY = origin.y + size.y / 2.0f;
		float centerZ = origin.z + size.z / 2.0f;

		// Convert angle from degrees to radians if necessary
		float radians = glm::radians(angle);

		glm::vec3 atopLeft(origin.x, origin.y, origin.z);
		glm::vec3 abottomLeft(origin.x, origin.y + size.y, origin.z);
		glm::vec3 abottomRight(origin.x + size.x, origin.y + size.y, origin.z);
		glm::vec3 atopRight(origin.x + size.x, origin.y, origin.z);

	/*	glm::vec3 arotatedTopLeft = rotatePoint(atopLeft.x, atopLeft.y, atopLeft.z, centerX, centerY, centerZ, 0, 0, 0);
		glm::vec3 arotatedBottomLeft = rotatePoint(abottomLeft.x, abottomLeft.y, abottomLeft.z, centerX, centerY, centerZ, 0, 0, 0);
		glm::vec3 arotatedBottomRight = rotatePoint(abottomRight.x, abottomRight.y, abottomRight.z, centerX, centerY, centerZ, 0, 0, 0);
		glm::vec3 arotatedTopRight = rotatePoint(atopRight.x, atopRight.y, atopRight.z, centerX, centerY, centerZ, 0, 0, 0);*/

		a_topLeft.color = color;
		a_topLeft.setPosition(atopLeft.x, atopLeft.y, atopLeft.z);

		a_bottomLeft.color = color;
		a_bottomLeft.setPosition(abottomLeft.x, abottomLeft.y, abottomLeft.z);

		a_bottomRight.color = color;
		a_bottomRight.setPosition(abottomRight.x, abottomRight.y, abottomRight.z);

		a_topRight.color = color;
		a_topRight.setPosition(atopRight.x, atopRight.y, atopRight.z);

		glm::vec3 btopLeft(origin.x, origin.y, origin.z + size.z);
		glm::vec3 bbottomLeft(origin.x, origin.y + size.y, origin.z + size.z);
		glm::vec3 bbottomRight(origin.x + size.x, origin.y + size.y, origin.z + size.z);
		glm::vec3 btopRight(origin.x + size.x, origin.y, origin.z + size.z);

	/*	glm::vec3 brotatedTopLeft = rotatePoint(btopLeft.x, btopLeft.y, btopLeft.z, centerX, centerY, centerZ, 0, 0, 0);
		glm::vec3 brotatedBottomLeft = rotatePoint(bbottomLeft.x, bbottomLeft.y, bbottomLeft.z, centerX, centerY, centerZ, 0, 0, 0);
		glm::vec3 brotatedBottomRight = rotatePoint(bbottomRight.x, bbottomRight.y, bbottomRight.z, centerX, centerY, centerZ, 0, 0, 0);
		glm::vec3 brotatedTopRight = rotatePoint(btopRight.x, btopRight.y, btopRight.z, centerX, centerY, centerZ, 0, 0, 0);*/

		b_topLeft.color = color;
		b_topLeft.setPosition(btopLeft.x, btopLeft.y, btopLeft.z);

		b_bottomLeft.color = color;
		b_bottomLeft.setPosition(bbottomLeft.x, bbottomLeft.y, bbottomLeft.z);

		b_bottomRight.color = color;
		b_bottomRight.setPosition(bbottomRight.x, bbottomRight.y, bbottomRight.z);

		b_topRight.color = color;
		b_topRight.setPosition(btopRight.x, btopRight.y, btopRight.z);

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

private:
	void createRenderBatches();
	void createVertexArray();

	GLuint _vbo = 0, _vao = 0, _ibo = 0; //! ibo is what is going to store the integers for each 

	std::vector<ColorVertex> _vertices;
	std::vector<GLuint> _indices;

	std::vector<LineGlyph>	_lineGlyphs;

	std::vector<SquareGlyph> _squareGlyphs;

	std::vector<BoxGlyph> _boxGlyphs;

	std::vector<RenderLineBatch> _renderBatches;
};