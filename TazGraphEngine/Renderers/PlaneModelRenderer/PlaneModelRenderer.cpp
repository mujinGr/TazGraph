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

	_glyphPointers.clear();
	_glyphs.clear();

	_triangleGlyphPointers.clear();
	_triangleGlyphs.clear();
}
void PlaneModelRenderer::end() {
	//set up all pointers for fast sorting
	_glyphPointers.resize(_glyphs.size());
	for (int i = 0; i < _glyphs.size(); i++) {
		_glyphPointers[i] = &_glyphs[i];
	}
	_triangleGlyphPointers.resize(_triangleGlyphs.size());
	for (int i = 0; i < _triangleGlyphs.size(); i++) {
		_triangleGlyphPointers[i] = &_triangleGlyphs[i];
	}
	sortGlyphs();

	createRenderBatches();
}


void PlaneModelRenderer::initTriangleBatch(size_t mSize)
{
	_triangleGlyphs.reserve(mSize);
}

void PlaneModelRenderer::initQuadBatch(size_t mSize)
{
	_glyphs.reserve(mSize);
}

void PlaneModelRenderer::drawTriangle(
	const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
	const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3,
	GLuint texture, const Color& color, float angle = 0.f
) {
	_triangleGlyphs.emplace_back(v1,v2,v3,uv1,uv2,uv3, texture, color, angle);
}
// we can generalize the renderer for multiple kinds of meshes (triangle made instead of planes) by creating
// more draw functions for those meshes (like draw function for triangle).
// Also instead of glyphs have triangles, so when its time to createRenderBatches we see the next mesh
// how many triangles it has and accordingly add those multiple vertices with the combined texture
void PlaneModelRenderer::draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const Color& color, float angle) {
	_glyphs.emplace_back(destRect, uvRect, texture, depth, color, angle);
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
	vertices.resize((_glyphPointers.size() * RECT_OFFSET) + (_triangleGlyphPointers.size() * TRIANGLE_OFFSET));
	if (_glyphPointers.empty() && _triangleGlyphPointers.empty()) {
		return;
	}

	int offset = 0;
	int cv = 0; //current vertex
	if (_glyphPointers.size())
	{
		_renderBatches.emplace_back(offset, RECT_OFFSET, _glyphPointers[0]->texture);
		vertices[cv++] = _glyphPointers[0]->topLeft;
		vertices[cv++] = _glyphPointers[0]->bottomLeft;
		vertices[cv++] = _glyphPointers[0]->bottomRight;
		vertices[cv++] = _glyphPointers[0]->bottomRight;
		vertices[cv++] = _glyphPointers[0]->topRight;
		vertices[cv++] = _glyphPointers[0]->topLeft;
		offset += RECT_OFFSET;

		for (int cg = 1; cg < _glyphPointers.size(); cg++) { //current Glyph
			if (_glyphPointers[cg]->texture != _glyphPointers[cg - 1]->texture) {
				_renderBatches.emplace_back(offset, RECT_OFFSET, _glyphPointers[0]->texture);
			}
			else {
				_renderBatches.back().numVertices += RECT_OFFSET;
			}
			vertices[cv++] = _glyphPointers[cg]->topLeft;
			vertices[cv++] = _glyphPointers[cg]->bottomLeft;
			vertices[cv++] = _glyphPointers[cg]->bottomRight;
			vertices[cv++] = _glyphPointers[cg]->bottomRight;
			vertices[cv++] = _glyphPointers[cg]->topRight;
			vertices[cv++] = _glyphPointers[cg]->topLeft;
			offset += RECT_OFFSET;
		}
	}

	if (_triangleGlyphPointers.size()) {
		_renderBatches.emplace_back(offset, TRIANGLE_OFFSET, _triangleGlyphPointers[0]->texture);
		vertices[cv++] = _triangleGlyphPointers[0]->topLeft;
		vertices[cv++] = _triangleGlyphPointers[0]->bottomLeft;
		vertices[cv++] = _triangleGlyphPointers[0]->bottomRight;
		offset += TRIANGLE_OFFSET;

		for (int cg = 1; cg < _triangleGlyphPointers.size(); cg++) { //current Glyph
			if (_triangleGlyphPointers[cg]->texture != _triangleGlyphPointers[cg - 1]->texture) {
				_renderBatches.emplace_back(offset, TRIANGLE_OFFSET, _triangleGlyphPointers[0]->texture);
			}
			else {
				_renderBatches.back().numVertices += TRIANGLE_OFFSET;
			}
			vertices[cv++] = _triangleGlyphPointers[cg]->topLeft;
			vertices[cv++] = _triangleGlyphPointers[cg]->bottomLeft;
			vertices[cv++] = _triangleGlyphPointers[cg]->bottomRight;
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

void PlaneModelRenderer::sortGlyphs() {
	switch (_sortType)
	{
	case GlyphSortType::NONE:
		break;
	case GlyphSortType::FRONT_TO_BACK:
		std::stable_sort(_glyphPointers.begin(), _glyphPointers.end(), compareFrontToBack);
		break;
	case GlyphSortType::BACK_TO_FRONT:
		std::stable_sort(_glyphPointers.begin(), _glyphPointers.end(), compareBackToFront);
		break;
	case GlyphSortType::TEXTURE:
		std::stable_sort(_glyphPointers.begin(), _glyphPointers.end(), compareTexture);
		break;
	default:
		break;
	}
}

bool PlaneModelRenderer::compareFrontToBack(Glyph* a, Glyph* b) {
	return (a->topLeft.position.z < b->topLeft.position.z);
}
bool PlaneModelRenderer::compareBackToFront(Glyph* a, Glyph* b) {
	return (a->topLeft.position.z > b->topLeft.position.z);
}
bool PlaneModelRenderer::compareTexture(Glyph* a, Glyph* b) {
	return (a->texture < b->texture);
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
