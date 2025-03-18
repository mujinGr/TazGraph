#include "PlaneModelRenderer.h"
#include <algorithm>

PlaneModelRenderer::PlaneModelRenderer() : _vbo(0), _vao(0) {

}

PlaneModelRenderer::~PlaneModelRenderer() {

}

void PlaneModelRenderer::init() {
	createVertexArray();
}

void PlaneModelRenderer::begin(GlyphSortType sortType/*GlyphSortType::TEXTURE*/) {
	_sortType = sortType;
	_renderBatches.clear();

	_glyphs.clear();

	_triangles.clear();
}
void PlaneModelRenderer::end() {
	//set up all pointers for fast sorting
	createRenderBatches();
}


void PlaneModelRenderer::initTriangleBatch(size_t mSize)
{
	_triangles.reserve(mSize);
}

void PlaneModelRenderer::initQuadBatch(size_t mSize)
{
	_glyphs.reserve(mSize);
}

void PlaneModelRenderer::drawTriangle(
	const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
	const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3,
	GLuint texture, const Color& color
) {
	_triangles.emplace_back(v1,v2,v3,uv1,uv2,uv3, texture, color);
}
// we can generalize the renderer for multiple kinds of meshes (triangle made instead of planes) by creating
// more draw functions for those meshes (like draw function for triangle).
// Also instead of glyphs have triangles, so when its time to createRenderBatches we see the next mesh
// how many triangles it has and accordingly add those multiple vertices with the combined texture
void PlaneModelRenderer::draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const Color& color) {
	_glyphs.emplace_back(destRect, uvRect, texture, depth, color);
}

void PlaneModelRenderer::renderBatch() {
	glBindVertexArray(_vao);

	for (int i = 0; i < _renderBatches.size(); i++) {
		glBindTexture(GL_TEXTURE_2D, _renderBatches[i].texture);

		glDrawArrays(GL_TRIANGLES, _renderBatches[i].offset, _renderBatches[i].numVertices);
	}

	glBindVertexArray(0);
}

void PlaneModelRenderer::createRenderBatches() {
	std::vector<Vertex> vertices;
	vertices.resize((_glyphs.size() * RECT_OFFSET) + (_triangles.size() * TRIANGLE_OFFSET));
	if (_glyphs.empty() && _triangles.empty()) {
		return;
	}

	int offset = 0;
	int cv = 0; //current vertex
	if (_glyphs.size())
	{
		_renderBatches.emplace_back(offset, RECT_OFFSET, _glyphs[0].texture);
		vertices[cv++] = _glyphs[0].topLeft;
		vertices[cv++] = _glyphs[0].bottomLeft;
		vertices[cv++] = _glyphs[0].bottomRight;
		vertices[cv++] = _glyphs[0].bottomRight;
		vertices[cv++] = _glyphs[0].topRight;
		vertices[cv++] = _glyphs[0].topLeft;
		offset += RECT_OFFSET;

		for (int cg = 1; cg < _glyphs.size(); cg++) { //current Glyph
			if (_glyphs[cg].texture != _glyphs[cg - 1].texture) {
				_renderBatches.emplace_back(offset, RECT_OFFSET, _glyphs[0].texture);
			}
			else {
				_renderBatches.back().numVertices += RECT_OFFSET;
			}
			vertices[cv++] = _glyphs[cg].topLeft;
			vertices[cv++] = _glyphs[cg].bottomLeft;
			vertices[cv++] = _glyphs[cg].bottomRight;
			vertices[cv++] = _glyphs[cg].bottomRight;
			vertices[cv++] = _glyphs[cg].topRight;
			vertices[cv++] = _glyphs[cg].topLeft;
			offset += RECT_OFFSET;
		}
	}

	if (_triangles.size()) {
		_renderBatches.emplace_back(offset, TRIANGLE_OFFSET, _triangles[0].texture);
		vertices[cv++] = _triangles[0].topLeft;
		vertices[cv++] = _triangles[0].bottomLeft;
		vertices[cv++] = _triangles[0].bottomRight;
		offset += TRIANGLE_OFFSET;

		for (int cg = 1; cg < _triangles.size(); cg++) { //current Glyph
			if (_triangles[cg].texture != _triangles[cg - 1].texture) {
				_renderBatches.emplace_back(offset, TRIANGLE_OFFSET, _triangles[0].texture);
			}
			else {
				_renderBatches.back().numVertices += TRIANGLE_OFFSET;
			}
			vertices[cv++] = _triangles[cg].topLeft;
			vertices[cv++] = _triangles[cg].bottomLeft;
			vertices[cv++] = _triangles[cg].bottomRight;
			offset += TRIANGLE_OFFSET;
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	//orphan the buffer / like using double buffer
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
	//upload the data
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());

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
