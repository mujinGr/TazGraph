#include "PlaneColorRenderer.h"
#include <algorithm>

PlaneColorRenderer::PlaneColorRenderer() : _vbo(0), _vao(0) {

}

PlaneColorRenderer::~PlaneColorRenderer() {

}

void PlaneColorRenderer::init() {
	createVertexArray();
}

void PlaneColorRenderer::begin(ColorGlyphSortType  sortType/*GlyphSortType::TEXTURE*/) {
	_sortType = sortType;
	_renderBatches.clear();

	_glyphPointers.clear();
	_glyphs.clear();

	_triangleGlyphPointers.clear();
	_triangleGlyphs.clear();
}
void PlaneColorRenderer::end() {
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


void PlaneColorRenderer::initColorTriangleBatch(size_t mSize)
{
	_triangleGlyphs.reserve(mSize);
}

void PlaneColorRenderer::initColorQuadBatch(size_t mSize)
{
	_glyphs.reserve(mSize);
}

void PlaneColorRenderer::drawTriangle(
	const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3,
	float depth, const Color& color, float angle = 0.f
) {
	_triangleGlyphs.emplace_back(v1, v2, v3, depth, color, angle);
}
// we can generalize the renderer for multiple kinds of meshes (triangle made instead of planes) by creating
// more draw functions for those meshes (like draw function for triangle).
// Also instead of glyphs have triangles, so when its time to createRenderBatches we see the next mesh
// how many triangles it has and accordingly add those multiple vertices with the combined texture
void PlaneColorRenderer::draw(const glm::vec4& destRect, float depth, const Color& color, float angle) {
	_glyphs.emplace_back(destRect, depth, color, angle);
}

void PlaneColorRenderer::renderBatch() {
	glBindVertexArray(_vao);

	for (int i = 0; i < _renderBatches.size(); i++) {
		glDrawArrays(GL_TRIANGLES, _renderBatches[i].offset, _renderBatches[i].numVertices);
	}

	glBindVertexArray(0);
}

void PlaneColorRenderer::createRenderBatches() {
	std::vector<ColorVertex> vertices;
	vertices.resize((_glyphPointers.size() * RECT_OFFSET) + (_triangleGlyphPointers.size() * TRIANGLE_OFFSET));
	if (_glyphPointers.empty() && _triangleGlyphPointers.empty()) {
		return;
	}

	int offset = 0;
	int cv = 0; //current vertex
	if (_glyphPointers.size())
	{
		_renderBatches.emplace_back(offset, RECT_OFFSET);
		vertices[cv++] = _glyphPointers[0]->topLeft;
		vertices[cv++] = _glyphPointers[0]->bottomLeft;
		vertices[cv++] = _glyphPointers[0]->bottomRight;
		vertices[cv++] = _glyphPointers[0]->bottomRight;
		vertices[cv++] = _glyphPointers[0]->topRight;
		vertices[cv++] = _glyphPointers[0]->topLeft;
		offset += RECT_OFFSET;

		for (int cg = 1; cg < _glyphPointers.size(); cg++) { //current Glyph
			_renderBatches.back().numVertices += RECT_OFFSET;
			
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
		_renderBatches.emplace_back(offset, TRIANGLE_OFFSET);
		vertices[cv++] = _triangleGlyphPointers[0]->topLeft;
		vertices[cv++] = _triangleGlyphPointers[0]->bottomLeft;
		vertices[cv++] = _triangleGlyphPointers[0]->bottomRight;
		offset += TRIANGLE_OFFSET;

		for (int cg = 1; cg < _triangleGlyphPointers.size(); cg++) { //current Glyph
			
			_renderBatches.back().numVertices += TRIANGLE_OFFSET;
			
			vertices[cv++] = _triangleGlyphPointers[cg]->topLeft;
			vertices[cv++] = _triangleGlyphPointers[cg]->bottomLeft;
			vertices[cv++] = _triangleGlyphPointers[cg]->bottomRight;
			offset += TRIANGLE_OFFSET;
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	//orphan the buffer / like using double buffer
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ColorVertex), nullptr, GL_DYNAMIC_DRAW);
	//upload the data
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(ColorVertex), vertices.data());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PlaneColorRenderer::createVertexArray() {

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

	glEnableVertexAttribArray(0); // give positions ( point to 0 element for position)
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	//position attribute pointer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ColorVertex), (void*)offsetof(ColorVertex, position)); // tell what data it is (first 0) and where the data is ( last 0 to go from the beggining)
	//color attribute pointer
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ColorVertex), (void*)offsetof(ColorVertex, color));
	
	glBindVertexArray(0);
}

void PlaneColorRenderer::sortGlyphs() {
	switch (_sortType)
	{
	case ColorGlyphSortType::NONE:
		break;
	case ColorGlyphSortType::FRONT_TO_BACK:
		std::stable_sort(_glyphPointers.begin(), _glyphPointers.end(), compareFrontToBack);
		break;
	case ColorGlyphSortType::BACK_TO_FRONT:
		std::stable_sort(_glyphPointers.begin(), _glyphPointers.end(), compareBackToFront);
		break;
	default:
		break;
	}
}

bool PlaneColorRenderer::compareFrontToBack(ColorGlyph* a, ColorGlyph* b) {
	return (a->depth < b->depth);
}
bool PlaneColorRenderer::compareBackToFront(ColorGlyph* a, ColorGlyph* b) {
	return (a->depth > b->depth);
}

void PlaneColorRenderer::dispose()
{
	if (_vao) {
		glDeleteVertexArrays(1, &_vao);
	}
	if (_vbo) {
		glDeleteBuffers(1, &_vbo);
	}
	//_program.dispose();
}
