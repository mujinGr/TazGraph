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
	LineGlyph(const glm::vec2& fromPosition, const glm::vec2& toPosition, const Color& srcColor, const Color& destColor, float mdepth) :
		depth(mdepth) {

		fromV.color = srcColor;
		fromV.setPosition(fromPosition.x, fromPosition.y, depth);

		toV.color = destColor;
		toV.setPosition(toPosition.x, toPosition.y, depth);
	};

	float depth;

	ColorVertex fromV;
	ColorVertex toV;
};

class SquareGlyph {

public:
	SquareGlyph() {};
	SquareGlyph(const glm::vec4& destRect, const Color& color, float angle, float mdepth) :
		depth(mdepth) {

		float centerX = destRect.x + destRect.z / 2.0f;
		float centerY = destRect.y + destRect.w / 2.0f;

		// Convert angle from degrees to radians if necessary
		float radians = glm::radians(angle);


		auto rotatePoint = [&](float x, float y) -> glm::vec2 {
			float dx = x - centerX;
			float dy = y - centerY;
			return glm::vec2(
				centerX + dx * cos(radians) - dy * sin(radians),
				centerY + dx * sin(radians) + dy * cos(radians)
			);
			};

		glm::vec2 rotatedTopLeft = rotatePoint(destRect.x, destRect.y);
		glm::vec2 rotatedBottomLeft = rotatePoint(destRect.x, destRect.y + destRect.w);
		glm::vec2 rotatedBottomRight = rotatePoint(destRect.x + destRect.z, destRect.y + destRect.w);
		glm::vec2 rotatedTopRight = rotatePoint(destRect.x + destRect.z, destRect.y);

		topLeft.color = color;
		topLeft.setPosition(rotatedTopLeft.x, rotatedTopLeft.y, depth);

		bottomLeft.color = color;
		bottomLeft.setPosition(rotatedBottomLeft.x, rotatedBottomLeft.y, depth);

		bottomRight.color = color;
		bottomRight.setPosition(rotatedBottomRight.x, rotatedBottomRight.y, depth);

		topRight.color = color;
		topRight.setPosition(rotatedTopRight.x, rotatedTopRight.y, depth);

	};

	float depth;

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

	void drawLine(const glm::vec2 srcPosition, const glm::vec2 destPosition, const Color& srcColor, const Color& destColor, float mDepth);
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