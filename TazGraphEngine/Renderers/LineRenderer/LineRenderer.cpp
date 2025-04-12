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

void LineRenderer::begin()
{
	_renderBatches.clear();

	_lineGlyphs_size = 0;
	_squareGlyphs_size = 0;
	_boxGlyphs_size = 0;

	_lines_verticesOffset = 0;
	_rectangles_verticesOffset = 0;
	_boxes_verticesOffset = 0;

	_lines_indicesOffset = 0;
	_rectangles_indicesOffset = 0;
	_boxes_indicesOffset = 0;

	_vertices.clear();
	_indices.clear();
}

void LineRenderer::end() // on end clear all indices reserved
{
	createRenderBatches();
}


void LineRenderer::initBatchLines(size_t msize)
{
	_lineGlyphs_size = msize;
}

void LineRenderer::initBatchSquares(size_t msize)
{
	_squareGlyphs_size = msize;
}

void LineRenderer::initBatchBoxes(size_t msize)
{
	_boxGlyphs_size = msize;
}

void LineRenderer::initBatchSize()
{
	if (_lineGlyphs_size!= 0) _renderBatches.push_back(RenderLineBatch(0, _lineGlyphs_size * INDICES_LINE_OFFSET));
	if (_squareGlyphs_size != 0) _renderBatches.push_back(RenderLineBatch(_lineGlyphs_size * INDICES_LINE_OFFSET, _squareGlyphs_size * INDICES_SQUARE_OFFSET));
	if (_boxGlyphs_size != 0) _renderBatches.push_back(RenderLineBatch(
		_lineGlyphs_size * INDICES_LINE_OFFSET + _squareGlyphs_size * INDICES_SQUARE_OFFSET,
		_boxGlyphs_size * INDICES_BOX_OFFSET
	));
	

	_vertices.resize(
		_lineGlyphs_size * LINE_OFFSET +
		_squareGlyphs_size * SQUARE_OFFSET +
		_boxGlyphs_size * BOX_OFFSET);
	_indices.resize(
		_lineGlyphs_size * INDICES_LINE_OFFSET +
		_squareGlyphs_size * INDICES_SQUARE_OFFSET +
		_boxGlyphs_size * INDICES_BOX_OFFSET);

	_lines_verticesOffset = 0;
	_rectangles_verticesOffset = _lineGlyphs_size * LINE_OFFSET;
	_boxes_verticesOffset = _lineGlyphs_size * LINE_OFFSET + _squareGlyphs_size * SQUARE_OFFSET;

	_lines_indicesOffset = 0;
	_rectangles_indicesOffset = _lineGlyphs_size * INDICES_LINE_OFFSET;
	_boxes_indicesOffset = _lineGlyphs_size * INDICES_LINE_OFFSET + _squareGlyphs_size * INDICES_SQUARE_OFFSET;
}

// todo can be optimized, by having something like glyphs in planeModelRenederer where first you pass info in a vector and
// todo on render pass that info in verts and indices
void LineRenderer::drawLine(size_t v_index, const glm::vec3 srcPosition, const glm::vec3 destPosition, const Color& srcColor, const Color& destColor)
{
	LineGlyph lineGlyph = LineGlyph(srcPosition, destPosition, srcColor, destColor);

	size_t i_cg = _lines_indicesOffset + v_index * INDICES_LINE_OFFSET;
	size_t verts_index = _lines_verticesOffset + v_index * LINE_OFFSET;

	int cv = i_cg;
	
	_indices[i_cg++] = cv;
	_vertices[verts_index++] = lineGlyph.fromV;
	_indices[i_cg++] = ++cv;
	_vertices[verts_index] = lineGlyph.toV;

}

void LineRenderer::drawRectangle(size_t v_index, const glm::vec4& destRect, const Color& color, float angle, float zIndex)
{
	SquareGlyph squareGlyph = SquareGlyph(destRect, color, angle, zIndex);
	
	size_t i_cg = _rectangles_indicesOffset + v_index * INDICES_SQUARE_OFFSET;
	size_t verts_index = _rectangles_verticesOffset + v_index * SQUARE_OFFSET;

	int cv = verts_index;

	_indices[i_cg++] = cv;
	_indices[i_cg++] = cv + 1;
	_vertices[verts_index++] = squareGlyph.topLeft;

	_indices[i_cg++] = ++cv;
	_indices[i_cg++] = cv + 1;
	_vertices[verts_index++] = squareGlyph.bottomLeft;

	_indices[i_cg++] = ++cv;
	_indices[i_cg++] = cv + 1;
	_vertices[verts_index++] = squareGlyph.bottomRight;

	_indices[i_cg++] = ++cv;
	_indices[i_cg++] = cv - SQUARE_OFFSET + 1;
	_vertices[verts_index++] = squareGlyph.topRight;
}

void LineRenderer::drawBox(size_t v_index, const glm::vec3& origin, const glm::vec3& size, const Color& color, float angle)
{
	BoxGlyph boxGlyph = BoxGlyph(origin, size, color, angle);

	size_t i_cg = _boxes_indicesOffset + v_index * INDICES_BOX_OFFSET;
	size_t verts_index = _boxes_verticesOffset + v_index * BOX_OFFSET;

	int cv = verts_index;

	_vertices[verts_index++] = boxGlyph.a_topLeft;
	_vertices[verts_index++] = boxGlyph.a_bottomLeft;
	_vertices[verts_index++] = boxGlyph.a_bottomRight;
	_vertices[verts_index++] = boxGlyph.a_topRight;

	_vertices[verts_index++] = boxGlyph.b_topLeft;
	_vertices[verts_index++] = boxGlyph.b_bottomLeft;
	_vertices[verts_index++] = boxGlyph.b_bottomRight;
	_vertices[verts_index++] = boxGlyph.b_topRight;

	for (int i = 0; i < 12; i++) {
		_indices[i_cg++] = cv + LineRenderer::box_edgePairs[i][0];
		_indices[i_cg++] = cv + LineRenderer::box_edgePairs[i][1];
	}

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
		glDrawElements(GL_LINES, _renderBatches[i].numIndices, GL_UNSIGNED_INT, (void*)(_renderBatches[i].offset * sizeof(GLuint)));
	}
	glBindVertexArray(0);
}


void LineRenderer::createRenderBatches() {

	if (_lineGlyphs_size == 0 && _squareGlyphs_size == 0 && _boxGlyphs_size == 0) {
		return;
	}

	//Bind buffer for the information for the vertexes
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	//Orphan the buffer : replace the memory block for the buffer object, 
	//dont wait the gpu for operations on the old data
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(ColorVertex), nullptr, GL_DYNAMIC_DRAW);
	//Upload the data
	glBufferSubData(GL_ARRAY_BUFFER, 0, _vertices.size() * sizeof(ColorVertex), _vertices.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Bind buffer for the indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
	//Orphan the buffer : replace the memory block for the buffer object, 
	//dont wait the gpu for operations on the old data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
	//Upload the data
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, _indices.size() * sizeof(GLuint), _indices.data());
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
