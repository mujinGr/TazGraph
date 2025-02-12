#include "LineRenderer.h"
#include <algorithm>

constexpr float PI = 3.14159265358f;

LineRenderer::LineRenderer()
{
}

LineRenderer::~LineRenderer()
{
	dispose();
}

void LineRenderer::init()
{
	createVertexArray();
}

void LineRenderer::begin(LineGlyphSortType sortType)
{
	_sortType = sortType;
	_renderBatches.clear();

	_lineGlyphPointers.clear();
	_lineGlyphs.clear();
}

void LineRenderer::end() // on end clear all indices reserved
{
	//set up all pointers for fast sorting
	_lineGlyphPointers.resize(_lineGlyphs.size());
	for (int i = 0; i < _lineGlyphs.size(); i++) {
		_lineGlyphPointers[i] = &_lineGlyphs[i];
	}

	sortGlyphs();

	createRenderBatches();
}


void LineRenderer::initBatch(size_t msize)
{
	_lineGlyphs.reserve(msize);
}
// todo can be optimized, by having something like glyphs in planeModelRenederer where first you pass info in a vector and
// todo on render pass that info in verts and indices
void LineRenderer::drawLine(const glm::vec2 srcPosition, const glm::vec2 destPosition, const Color& srcColor, const Color& destColor, float mdepth)
{
	_lineGlyphs.emplace_back(srcPosition, destPosition, srcColor, destColor, mdepth);
}


void LineRenderer::drawBox(const glm::vec4& destRect, const Color& color, float angle, float zIndex)
{
	//int i = _verts.size();
	//_verts.resize(_verts.size() + 4); // more efficient than calling pushBack 4 times

	//float centerX = destRect.x + destRect.z / 2.0f;
	//float centerY = destRect.y + destRect.w / 2.0f;

	//// Convert angle from degrees to radians if necessary
	//float radians = glm::radians(angle);

	//// Lambda to calculate rotated positions
	//auto rotatePoint = [&](float x, float y) -> glm::vec3 {
	//	float dx = x - centerX;
	//	float dy = y - centerY;
	//	return glm::vec3(
	//		centerX + dx * cos(radians) - dy * sin(radians),
	//		centerY + dx * sin(radians) + dy * cos(radians),
	//		zIndex
	//	);
	//	};

	//glm::vec2 positionOffset(destRect.x , destRect.y );

	//_verts[  i  ].setPosition( rotatePoint(positionOffset.x, positionOffset.y + destRect.w));
	//_verts[i + 1].setPosition(rotatePoint(positionOffset.x, positionOffset.y));
	//_verts[i + 2].setPosition(rotatePoint(positionOffset.x + destRect.z, positionOffset.y));
	//_verts[i + 3].setPosition(rotatePoint(positionOffset.x + destRect.z, positionOffset.y + destRect.w));

	//for (int j = i; j < i + 4; j++) {
	//	_verts[j].color = color;
	//}

	//_indices.reserve(_indices.size() + 8); // indices for the ibo // + the number of verts in total

	//_indices.push_back(  i  );
	//_indices.push_back(i + 1);

	//_indices.push_back(i + 1);
	//_indices.push_back(i + 2);

	//_indices.push_back(i + 2);
	//_indices.push_back(i + 3);

	//_indices.push_back(i + 3);
	//_indices.push_back(  i  );

}

void LineRenderer::drawCircle(const glm::vec2& center, const Color& color, float radius)
{
	//static const int NUM_VERTS = 255;
	//// Set up vertices
	//int start = _verts.size();
	//_verts.resize(_verts.size() + NUM_VERTS);
	//for (int i = 0; i < NUM_VERTS; i++) {
	//	float angle = ((float)i / NUM_VERTS) * 2.0f * PI;

	//	_verts[start + i].position.x = cos(angle) * radius + center.x;
	//	_verts[start + i].position.y = sin(angle) * radius + center.y;
	//	_verts[start + i].color = color;
	//}

	//// Set up indexes for drawing
	//_indices.reserve(_indices.size() + NUM_VERTS * 2);

	//for (int i = 0; i < NUM_VERTS - 1; i++) {
	//	_indices.push_back(start + i);
	//	_indices.push_back(start + i + 1);
	//}
	//_indices.push_back(start + NUM_VERTS - 1);
	//_indices.push_back(start);
}


void LineRenderer::renderBatch(float lineWidth )
{
	glLineWidth(lineWidth);
	glBindVertexArray(_vao);

	for (int i = 0; i < _renderBatches.size(); i++) {
		glDrawElements(GL_LINES, _renderBatches[i].numIndices, GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);
}


void LineRenderer::createRenderBatches() {
	std::vector<ColorVertex> vertices;
	std::vector<GLuint> indices;

	vertices.resize(_lineGlyphPointers.size() * LINE_OFFSET);
	indices.resize(_lineGlyphPointers.size() * LINE_OFFSET);

	if (_lineGlyphPointers.empty()) {
		return;
	}

	int offset = 0;
	int cv = 0; //current vertex

	_renderBatches.emplace_back(offset, LINE_OFFSET);
	indices[cv] = cv;
	vertices[cv++] = _lineGlyphPointers[0]->fromV;
	indices[cv] = cv;
	vertices[cv++] = _lineGlyphPointers[0]->toV;

	offset += LINE_OFFSET;

	for (int cg = 1; cg < _lineGlyphPointers.size(); cg++) { //current Glyph

		_renderBatches.back().numIndices += LINE_OFFSET;

		indices[cv] = cv;
		vertices[cv++] = _lineGlyphPointers[cg]->fromV;
		indices[cv] = cv;
		vertices[cv++] = _lineGlyphPointers[cg]->toV;
		offset += LINE_OFFSET;
	}


	//Bind buffer for the information for the vertexes
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	//Orphan the buffer : replace the memory block for the buffer object, 
	//dont wait the gpu for operations on the old data
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ColorVertex), nullptr, GL_DYNAMIC_DRAW);
	//Upload the data
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(ColorVertex), vertices.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Bind buffer for the indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
	//Orphan the buffer : replace the memory block for the buffer object, 
	//dont wait the gpu for operations on the old data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
	//Upload the data
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(GLuint), indices.data());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void LineRenderer::createVertexArray() {

	//Set up buffers
	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
	glGenBuffers(1, &_ibo);

	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo); //Buffer used for index drawing

	glEnableVertexAttribArray(0);
	// 0 attrib, 2 floats, type, not normalised, sizeof Vertex, pointer to the Vertex Data
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ColorVertex), (void*)offsetof(ColorVertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ColorVertex), (void*)offsetof(ColorVertex, color));

	glBindVertexArray(0); //! the buffers are bound withing the context of vao so when that is
	//! unbinded all the other vbos are also unbinded
}

void LineRenderer::sortGlyphs() {
	switch (_sortType)
	{
	case LineGlyphSortType::NONE:
		break;
	case LineGlyphSortType::FRONT_TO_BACK:
		std::stable_sort(_lineGlyphPointers.begin(), _lineGlyphPointers.end(), [](LineGlyph* a, LineGlyph* b) {
			return (a->depth < b->depth);
			});
		break;
	case LineGlyphSortType::BACK_TO_FRONT:
		std::stable_sort(_lineGlyphPointers.begin(), _lineGlyphPointers.end(), [](LineGlyph* a, LineGlyph* b) {
			return (a->depth > b->depth);
			});
		break;
	default:
		break;
	}
}

void LineRenderer::dispose()
{
	if (_vao) {
		glDeleteVertexArrays(1, &_vao);
	}
	if (_vbo) {
		glDeleteBuffers(1, &_vbo);
	}
	if (_ibo) {
		glDeleteBuffers(1, &_ibo);
	}
	//_program.dispose();
}
