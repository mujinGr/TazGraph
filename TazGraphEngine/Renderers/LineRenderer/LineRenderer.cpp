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

	_lineGlyphs.clear();

	_squareGlyphs.clear();

	_boxGlyphs.clear();
}

void LineRenderer::end() // on end clear all indices reserved
{
	createRenderBatches();
}


void LineRenderer::initBatchLines(size_t msize)
{
	_lineGlyphs.resize(msize);
}

void LineRenderer::initBatchSquares(size_t msize)
{
	_squareGlyphs.resize(msize);
}

void LineRenderer::initBatchBoxes(size_t msize)
{
	_boxGlyphs.resize(msize);
}

void LineRenderer::initBatchSize()
{
	if (!_squareGlyphs.empty()) _renderBatches.push_back(RenderLineBatch(0, _squareGlyphs.size() * SQUARE_OFFSET * 2));
	if (!_lineGlyphs.empty()) _renderBatches.push_back(RenderLineBatch(_squareGlyphs.size() * SQUARE_OFFSET, _lineGlyphs.size() * LINE_OFFSET));
	if (!_boxGlyphs.empty()) _renderBatches.push_back(RenderLineBatch(
		_lineGlyphs.size() * LINE_OFFSET + _squareGlyphs.size() * SQUARE_OFFSET,
		_boxGlyphs.size() * BOX_OFFSET * 4
	));

	_vertices.resize(
		_lineGlyphs.size() * LINE_OFFSET +
		_squareGlyphs.size() * SQUARE_OFFSET +
		_boxGlyphs.size() * BOX_OFFSET);
	_indices.resize(
		_lineGlyphs.size() * LINE_OFFSET +
		_squareGlyphs.size() * SQUARE_OFFSET * 2 +
		_boxGlyphs.size() * BOX_OFFSET * 4);
}

// todo can be optimized, by having something like glyphs in planeModelRenederer where first you pass info in a vector and
// todo on render pass that info in verts and indices
void LineRenderer::drawLine(size_t v_index, const glm::vec3 srcPosition, const glm::vec3 destPosition, const Color& srcColor, const Color& destColor)
{
	LineGlyph lineGlyph = LineGlyph(srcPosition, destPosition, srcColor, destColor);

	int i_cg = _squareGlyphs.size() * 2 * SQUARE_OFFSET + v_index * LINE_OFFSET;
	int verts_index = _squareGlyphs.size() * SQUARE_OFFSET + v_index * LINE_OFFSET;

	int cv = i_cg;
	
	_indices[i_cg++] = cv;
	_vertices[verts_index++] = lineGlyph.fromV;
	_indices[i_cg++] = ++cv;
	_vertices[verts_index] = lineGlyph.toV;

}

void LineRenderer::drawRectangle(size_t v_index, const glm::vec4& destRect, const Color& color, float angle, float zIndex)
{
	_squareGlyphs[v_index] = SquareGlyph(destRect, color, angle, zIndex);
}

void LineRenderer::drawBox(size_t v_index, const glm::vec3& origin, const glm::vec3& size, const Color& color, float angle)
{
	_boxGlyphs[v_index] = BoxGlyph(origin, size, color, angle);
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

	
	if (_lineGlyphs.empty() && _squareGlyphs.empty() && _boxGlyphs.empty()) {
		return;
	}

	int offset = 0;
	int cv = -1; //current vertex

	int batchIndex = 0;
	if (_squareGlyphs.size()) {
		int i_cg = 0;
		int v_index = 0;

		_indices[i_cg++] = ++cv;
		_indices[i_cg++] = cv + 1;
		_vertices[v_index++] = _squareGlyphs[0].topLeft;

		_indices[i_cg++] = ++cv;
		_indices[i_cg++] = cv + 1;
		_vertices[v_index++] = _squareGlyphs[0].bottomLeft;
		
		_indices[i_cg++] = ++cv;
		_indices[i_cg++] = cv + 1;
		_vertices[v_index++] = _squareGlyphs[0].bottomRight;

		_indices[i_cg++] = ++cv;
		_indices[i_cg++] = cv - SQUARE_OFFSET + 1;
		_vertices[v_index++] = _squareGlyphs[0].topRight;

		offset += 2 * SQUARE_OFFSET;

		for (int cg = 1; cg < _squareGlyphs.size(); cg++) { //current Glyph
			_indices[i_cg++] = ++cv;
			_indices[i_cg++] = cv + 1;
			_vertices[v_index++] = _squareGlyphs[cg].topLeft;

			_indices[i_cg++] = ++cv;
			_indices[i_cg++] = cv + 1;
			_vertices[v_index++] = _squareGlyphs[cg].bottomLeft;

			_indices[i_cg++] = ++cv;
			_indices[i_cg++] = cv + 1;
			_vertices[v_index++] = _squareGlyphs[cg].bottomRight;

			_indices[i_cg++] = ++cv;
			_indices[i_cg++] = cv - SQUARE_OFFSET + 1;
			_vertices[v_index++] = _squareGlyphs[cg].topRight;

			offset += 2 * SQUARE_OFFSET;
		}
	}
	
	if (_boxGlyphs.size()) {
		int i_cg = _squareGlyphs.size() * 2 * SQUARE_OFFSET + _lineGlyphs.size() * LINE_OFFSET;

		int v_index = _squareGlyphs.size() * SQUARE_OFFSET + _lineGlyphs.size() * LINE_OFFSET;

		int cv_save = v_index;
		int edgePairs[12][2] = {
			{0, 1}, {1, 2}, {2, 3}, {3, 0}, // Bottom face
			{4, 5}, {5, 6}, {6, 7}, {7, 4}, // Top face
			{0, 4}, {1, 5}, {2, 6}, {3, 7}  // Vertical edges
		};

		_vertices[v_index++] = _boxGlyphs[0].a_topLeft;
		_vertices[v_index++] = _boxGlyphs[0].a_bottomLeft;
		_vertices[v_index++] = _boxGlyphs[0].a_bottomRight;
		_vertices[v_index++] = _boxGlyphs[0].a_topRight;
		
		_vertices[v_index++] = _boxGlyphs[0].b_topLeft;
		_vertices[v_index++] = _boxGlyphs[0].b_bottomLeft;
		_vertices[v_index++] = _boxGlyphs[0].b_bottomRight;
		_vertices[v_index++] = _boxGlyphs[0].b_topRight;

		for (int i = 0; i < 12; i++) {
			_indices[i_cg++] = cv_save + edgePairs[i][0];
			_indices[i_cg++] = cv_save + edgePairs[i][1];
		}

		offset += 3 * BOX_OFFSET;

		for (int cg = 1; cg < _boxGlyphs.size(); cg++) {

			i_cg = cg * 3 * BOX_OFFSET;

			cv_save = v_index;
			_vertices[v_index++] = _boxGlyphs[cg].a_topLeft;
			_vertices[v_index++] = _boxGlyphs[cg].a_bottomLeft;
			_vertices[v_index++] = _boxGlyphs[cg].a_bottomRight;
			_vertices[v_index++] = _boxGlyphs[cg].a_topRight;

			_vertices[v_index++] = _boxGlyphs[cg].b_topLeft;
			_vertices[v_index++] = _boxGlyphs[cg].b_bottomLeft;
			_vertices[v_index++] = _boxGlyphs[cg].b_bottomRight;
			_vertices[v_index++] = _boxGlyphs[cg].b_topRight;

			for (int i = 0; i < 12; i++) {
				_indices[i_cg++]		= cv_save + edgePairs[i][0];
				_indices[i_cg++]	= cv_save + edgePairs[i][1];
			}

			offset += 3 * BOX_OFFSET;

		}
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
