#include "PlaneModelRenderer.h"
#include <algorithm>

PlaneModelRenderer::PlaneModelRenderer() : _vbo(0), _vao(0),
_glyphs_size(0), _triangles_size(0),
_triangles_verticesOffset(0), _rectangles_verticesOffset(0)
{

}

PlaneModelRenderer::~PlaneModelRenderer() {

}

void PlaneModelRenderer::init() {
	createVertexArray();
}

void PlaneModelRenderer::begin() {
	_renderBatches.clear();

	_glyphs_size = 0;
	_triangles_size = 0;

	_triangles_verticesOffset = 0;
	_rectangles_verticesOffset = 0;


	_vertices.clear();
}
void PlaneModelRenderer::end() {
	//set up all pointers for fast sorting
	createRenderBatches();
}


void PlaneModelRenderer::initTextureTriangleBatch(size_t mSize)
{
	_triangles_size = mSize;
}

void PlaneModelRenderer::initTextureQuadBatch(size_t mSize)
{
	_glyphs_size = mSize;
}

void PlaneModelRenderer::initBatchSize()
{
	_rectangles_verticesOffset = 0;
	_triangles_verticesOffset = _glyphs_size * RECT_OFFSET;

	_vertices.resize((_glyphs_size * RECT_OFFSET) + (_triangles_size * TRIANGLE_OFFSET));
}

void PlaneModelRenderer::drawTriangle(
	size_t v_index,
	const glm::vec3& triangleOffset,
	const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
	const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3,
	GLuint texture, const Color& color
) {
	TriangleGlyph triangleGlyph = TriangleGlyph(v1, v2, v3, uv1, uv2, uv3, texture, color);

	int offset = _triangles_verticesOffset + v_index * TRIANGLE_OFFSET;

	_renderBatches.emplace_back(offset, TRIANGLE_OFFSET, triangleOffset, triangleGlyph.texture);

	_vertices[offset++] = triangleGlyph.topLeft;
	_vertices[offset++] = triangleGlyph.bottomLeft;
	_vertices[offset++] = triangleGlyph.bottomRight;
}
// we can generalize the renderer for multiple kinds of meshes (triangle made instead of planes) by creating
// more draw functions for those meshes (like draw function for triangle).
// Also instead of glyphs have triangles, so when its time to createRenderBatches we see the next mesh
// how many triangles it has and accordingly add those multiple vertices with the combined texture
void PlaneModelRenderer::draw(
	size_t v_index,
	const glm::vec3& rectOffset,
	const glm::vec2& rectSize,
	const glm::vec4& uvRect,
	GLuint texture, const glm::vec3& bodyCenter, const Color& color) {

	Glyph glyph = Glyph(rectSize, uvRect, texture, bodyCenter.z, color);

	int offset = v_index * RECT_OFFSET;

	_renderBatches.emplace_back(offset, RECT_OFFSET, rectOffset, glyph.texture);

	_vertices[offset++] = glyph.topLeft;
	_vertices[offset++] = glyph.bottomLeft;
	_vertices[offset++] = glyph.bottomRight;
	_vertices[offset++] = glyph.bottomRight;
	_vertices[offset++] = glyph.topRight;
	_vertices[offset++] = glyph.topLeft;

}

void PlaneModelRenderer::renderBatch(GLSLProgram* glsl_program) {
	glBindVertexArray(_vao);

	GLint centerPosLocation = glGetUniformLocation(glsl_program->getProgramID(), "centerPosition");

	for (int i = 0; i < _renderBatches.size(); i++) {
		glUniform3fv(centerPosLocation, 1, glm::value_ptr(_renderBatches[i].centerPos));
		
		if (i == 0 || _renderBatches[i-1].texture != _renderBatches[i].texture) {
			glBindTexture(GL_TEXTURE_2D, _renderBatches[i].texture);
		}

		glDrawArrays(GL_TRIANGLES, _renderBatches[i].offset, _renderBatches[i].numVertices);
	}

	glBindVertexArray(0);
}

void PlaneModelRenderer::createRenderBatches() {

	if (_glyphs_size == 0 && _triangles_size == 0) {
		return;
	}

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	//orphan the buffer / like using double buffer
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
	//upload the data
	glBufferSubData(GL_ARRAY_BUFFER, 0, _vertices.size() * sizeof(Vertex), _vertices.data());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PlaneModelRenderer::createVertexArray() {

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

	glEnableVertexAttribArray(0); // give positions ( point to 0 element for position)
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	//position attribute pointer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position)); // tell what data it is (first 0) and where the data is ( last 0 to go from the beggining)
	//color attribute pointer
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	// UV attribute pointer
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

	glBindVertexArray(0);
}

void PlaneModelRenderer::dispose()
{
	if (_vao) {
		glDeleteVertexArrays(1, &_vao);
	}
	if (_vbo) {
		glDeleteBuffers(1, &_vbo);
	}
	//_program.dispose();
}
