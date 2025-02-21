#pragma once

#include "../../GLSLProgram.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#include "../../Vertex.h"

#define LINE_OFFSET 2
#define SQUARE_OFFSET 4

enum class LineGlyphSortType {
	NONE,
	FRONT_TO_BACK,
	BACK_TO_FRONT
};

class RenderLineBatch {
public:
	RenderLineBatch(GLuint Offset, GLuint NumIndices) : offset(Offset),
		numIndices(NumIndices){

	}
	GLuint offset;
	GLuint numIndices;
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

		glm::vec3 rotatedTopLeft = rotatePoint(atopLeft.x, atopLeft.y, atopLeft.z, centerX, centerY, centerZ, 0, 0, 0);
		glm::vec3 rotatedBottomLeft = rotatePoint(abottomLeft.x, abottomLeft.y, abottomLeft.z, centerX, centerY, centerZ, 0, 0, 0);
		glm::vec3 rotatedBottomRight = rotatePoint(abottomRight.x, abottomRight.y, abottomRight.z, centerX, centerY, centerZ, 0, 0, 0);
		glm::vec3 rotatedTopRight = rotatePoint(atopRight.x, atopRight.y, atopRight.z, centerX, centerY, centerZ, 0, 0, 0);

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
	void begin(LineGlyphSortType sortType = LineGlyphSortType::BACK_TO_FRONT);

	void end();

	void drawLine(const glm::vec3 srcPosition, const glm::vec3 destPosition, const Color& srcColor, const Color& destColor);
	void drawBox(const glm::vec4& destRect, const Color& color, float angle, float zIndex =0.0f);
	void drawCircle(const glm::vec2& center, const Color& color, float radius);

	void initBatch(size_t msize);
	void renderBatch(float lineWidth);
	
	void dispose();

private:
	void createRenderBatches();
	void createVertexArray();
	void sortGlyphs();

	GLuint _vbo = 0, _vao = 0, _ibo = 0; //! ibo is what is going to store the integers for each 

	LineGlyphSortType _sortType;

	std::vector<LineGlyph*> _lineGlyphPointers;
	std::vector<LineGlyph>	_lineGlyphs;

	std::vector<SquareGlyph*> _squareGlyphPointers;
	std::vector<SquareGlyph> _squareGlyphs;

	std::vector<RenderLineBatch> _renderBatches;
};