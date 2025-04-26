#include "PlaneColorRenderer.h"
#include <algorithm>

PlaneColorRenderer::PlaneColorRenderer() : _vbo(0), _vao(0),
_glyphs_size(0), _triangleGlyphs_size(0), _boxGlyphs_size(0){

}

PlaneColorRenderer::~PlaneColorRenderer() {

}

void PlaneColorRenderer::init() {
	createVertexArray();
}

void PlaneColorRenderer::begin() {
	_renderBatchesArrays.clear();

	_glyphs_size = 0;
	_triangleGlyphs_size = 0;
	_boxGlyphs_size = 0;

	_triangles_verticesOffset = 0;
	_rectangles_verticesOffset = 0;

	_vertices.clear();
}
void PlaneColorRenderer::end() {

	//set up all pointers for fast sorting
	createRenderBatches();
}


void PlaneColorRenderer::initColorTriangleBatch(size_t mSize)
{
	_triangleGlyphs_size = mSize;
}

void PlaneColorRenderer::initColorQuadBatch(size_t mSize)
{
	_glyphs_size = mSize;
}


void PlaneColorRenderer::initBatchSize()
{
	size_t totalGlyphs = _glyphs_size + _triangleGlyphs_size;

	_rectangles_verticesOffset = 0;
	_triangles_verticesOffset = _glyphs_size * SQUARE_OFFSET;

	_renderBatchesArrays.resize(totalGlyphs);
	_renderBatchesElements.resize(totalGlyphs);
	_vertices.resize((_glyphs_size * SQUARE_OFFSET) + (_triangleGlyphs_size * TRIANGLE_OFFSET));
	_indices.resize((_glyphs_size * SQUARE_PLANE_INDICES_OFFSET));
}


void PlaneColorRenderer::initColorBoxBatch(size_t mSize)
{
	_boxGlyphs_size = mSize;
}

void PlaneColorRenderer::drawTriangle(
	size_t v_index,
	const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3,
	const glm::vec3& bodyCenter, const glm::vec3& cpuRotation, 
	const Color& color
) {
	TriangleColorGlyph triangleGlyph = TriangleColorGlyph(v1, v2, v3, cpuRotation, color);

	int offset =  _triangles_verticesOffset + v_index * TRIANGLE_OFFSET;

	v_index = v_index + _glyphs_size;

	_renderBatchesArrays[v_index] = ColorRenderBatchArrays(
		offset,
		TRIANGLE_OFFSET,
		bodyCenter);
	_vertices[offset++] = triangleGlyph.topLeft;
	_vertices[offset++] = triangleGlyph.bottomLeft;
	_vertices[offset++] = triangleGlyph.bottomRight;
}
// we can generalize the renderer for multiple kinds of meshes (triangle made instead of planes) by creating
// more draw functions for those meshes (like draw function for triangle).
// Also instead of glyphs have triangles, so when its time to createRenderBatches we see the next mesh
// how many triangles it has and accordingly add those multiple vertices with the combined texture
//! draws are needed to convert the pos and size to vertices
void PlaneColorRenderer::draw(size_t v_index, const glm::vec2& rectSize, const glm::vec3& bodyCenter, const Color& color) {
	ColorGlyph glyph = ColorGlyph(rectSize, color);

	int offset = v_index * SQUARE_OFFSET;
	int index_offset = v_index * SQUARE_PLANE_INDICES_OFFSET;

	_renderBatchesElements[v_index] = ColorRenderBatchElements(index_offset, SQUARE_PLANE_INDICES_OFFSET, bodyCenter);
	
	_indices[index_offset++] = offset + rect_triangleIndices[0][0];
	_indices[index_offset++] = offset + rect_triangleIndices[0][1];
	_indices[index_offset++] = offset + rect_triangleIndices[0][2];
	
	_indices[index_offset++] = offset + rect_triangleIndices[1][0];
	_indices[index_offset++] = offset + rect_triangleIndices[1][1];
	_indices[index_offset++] = offset + rect_triangleIndices[1][2];

	_vertices[offset++] = glyph.topLeft;
	_vertices[offset++] = glyph.bottomLeft;
	_vertices[offset++] = glyph.bottomRight;
	_vertices[offset++] = glyph.topRight;

}

void PlaneColorRenderer::drawBox(const glm::vec4& destRect, float depth, const Color& color) {
	//_boxGlyphs.emplace_back(destRect, depth, color);
}

void PlaneColorRenderer::renderBatch(GLSLProgram* glsl_program) {
	glBindVertexArray(_vao);

	GLint centerPosLocation = glGetUniformLocation(glsl_program->getProgramID(), "centerPosition");

	for (int i = 0; i < _renderBatchesElements.size(); i++) {
		glUniform3fv(centerPosLocation, 1, glm::value_ptr(_renderBatchesElements[i].centerPos));

		glDrawElements(
			GL_TRIANGLES,
			_renderBatchesElements[i].numIndices,
			GL_UNSIGNED_INT,
			(void*)(_renderBatchesElements[i].offset * sizeof(GLuint))
		);
	}
	for (int i = 0; i < _renderBatchesArrays.size(); i++) {
		glUniform3fv(centerPosLocation, 1, glm::value_ptr(_renderBatchesArrays[i].centerPos));

		glDrawArrays(
			GL_TRIANGLES,
			_renderBatchesArrays[i].offset,
			_renderBatchesArrays[i].numVertices
		);
	}


	glBindVertexArray(0);
}

void PlaneColorRenderer::createRenderBatches() {

	//current vertex
	if ((_glyphs_size + _triangleGlyphs_size) == 0) {
		return;
	}
	// todo not complete
	/*if (_boxGlyphPointers.size()) {
	
	}*/

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(	GL_ARRAY_BUFFER,			_vertices.size() * sizeof(ColorVertex),	nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER,			0,	_vertices.size() * sizeof(ColorVertex), _vertices.data());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
	glBufferData(	GL_ELEMENT_ARRAY_BUFFER,	_indices.size() * sizeof(GLuint),		nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,	0,	_indices.size() * sizeof(GLuint),		_indices.data());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void PlaneColorRenderer::createVertexArray() {

	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
	glGenBuffers(1, &_ibo);

	glBindVertexArray(_vao);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo); //Buffer used for index drawing

	glEnableVertexAttribArray(0); // give positions ( point to 0 element for position)
	//position attribute pointer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ColorVertex), (void*)offsetof(ColorVertex, position)); // tell what data it is (first 0) and where the data is ( last 0 to go from the beggining)
	glEnableVertexAttribArray(1);
	//color attribute pointer
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ColorVertex), (void*)offsetof(ColorVertex, color));
	
	glBindVertexArray(0);
}

void PlaneColorRenderer::dispose()
{
	if (_vao) {
		glDeleteVertexArrays(1, &_vao);
	}
	if (_vbo) {
		glDeleteBuffers(1, &_vbo);
	}
	if (_ibo) {
		glDeleteBuffers(1, &_vbo);
	}
	//_program.dispose();
}
