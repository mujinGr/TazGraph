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

	_squareGlyphPointers.clear();
	_squareGlyphs.clear();
}

void LineRenderer::end() // on end clear all indices reserved
{
	//set up all pointers for fast sorting
	_lineGlyphPointers.resize(_lineGlyphs.size());
	for (int i = 0; i < _lineGlyphs.size(); i++) {
		_lineGlyphPointers[i] = &_lineGlyphs[i];
	}
	_squareGlyphPointers.resize(_squareGlyphs.size());
	for (int i = 0; i < _squareGlyphs.size(); i++) {
		_squareGlyphPointers[i] = &_squareGlyphs[i];
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
void LineRenderer::drawLine(const glm::vec3 srcPosition, const glm::vec3 destPosition, const Color& srcColor, const Color& destColor)
{
	_lineGlyphs.emplace_back(srcPosition, destPosition, srcColor, destColor);
}


void LineRenderer::drawBox(const glm::vec4& destRect, const Color& color, float angle, float zIndex)
{
	_squareGlyphs.emplace_back(destRect, color, angle, zIndex);
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

	vertices.resize(_lineGlyphPointers.size() * LINE_OFFSET + _squareGlyphPointers.size() * SQUARE_OFFSET);
	indices.resize(_lineGlyphPointers.size() * LINE_OFFSET + _squareGlyphPointers.size() * SQUARE_OFFSET * 2);

	if (_lineGlyphPointers.empty() && _squareGlyphPointers.empty()) {
		return;
	}

	int offset = 0;
	int cv = 0; //current vertex

	if (_squareGlyphPointers.size()) {
		_renderBatches.emplace_back(offset, 2 * SQUARE_OFFSET);

		indices[cv * 2] = cv;
		indices[cv * 2 + 1] = cv + 1;
		vertices[cv++] = _squareGlyphPointers[0]->topLeft;

		indices[cv * 2] = cv;
		indices[cv * 2 + 1] = cv + 1;
		vertices[cv++] = _squareGlyphPointers[0]->bottomLeft;

		indices[cv * 2] = cv;
		indices[cv * 2 + 1] = cv + 1;
		vertices[cv++] = _squareGlyphPointers[0]->bottomRight;

		indices[cv * 2] = cv;
		indices[cv * 2 + 1] = cv - SQUARE_OFFSET + 1;
		vertices[cv++] = _squareGlyphPointers[0]->topRight;

		offset += 2 * SQUARE_OFFSET;

		for (int cg = 1; cg < _squareGlyphPointers.size(); cg++) { //current Glyph

			_renderBatches.back().numIndices += 2 * SQUARE_OFFSET;

			indices[cv * 2] = cv;
			indices[cv * 2 + 1] = cv + 1;
			vertices[cv++] = _squareGlyphPointers[cg]->topLeft;

			indices[cv * 2] = cv;
			indices[cv * 2 + 1] = cv + 1;
			vertices[cv++] = _squareGlyphPointers[cg]->bottomLeft;

			indices[cv * 2] = cv;
			indices[cv * 2 + 1] = cv + 1;
			vertices[cv++] = _squareGlyphPointers[cg]->bottomRight;

			indices[cv * 2] = cv;
			indices[cv * 2 + 1] = cv - SQUARE_OFFSET + 1;
			vertices[cv++] = _squareGlyphPointers[cg]->topRight;

			offset += 2 * SQUARE_OFFSET;
		}
	}
	if (_lineGlyphPointers.size()) {
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
	// 0 attrib, 3 floats, type, not normalised, sizeof Vertex, pointer to the Vertex Data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ColorVertex), (void*)offsetof(ColorVertex, position));
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
			return (a->fromV.position.z < b->fromV.position.z);
			});
		break;
	case LineGlyphSortType::BACK_TO_FRONT:
		std::stable_sort(_lineGlyphPointers.begin(), _lineGlyphPointers.end(), [](LineGlyph* a, LineGlyph* b) {
			return (a->fromV.position.z > b->fromV.position.z);
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
