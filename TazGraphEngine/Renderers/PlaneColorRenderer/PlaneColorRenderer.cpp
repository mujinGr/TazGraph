#include "PlaneColorRenderer.h"
#include <algorithm>

PlaneColorRenderer::PlaneColorRenderer() : _vbo(0), _vao(0) {

}

PlaneColorRenderer::~PlaneColorRenderer() {

}

void PlaneColorRenderer::setThreader(Threader& mthreader)
{
	_threader = &mthreader;
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

	_boxGlyphPointers.clear();
	_boxGlyphs.clear();
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
	_boxGlyphPointers.resize(_boxGlyphs.size());
	for (int i = 0; i < _boxGlyphs.size(); i++) {
		_boxGlyphPointers[i] = &_boxGlyphs[i];
	}
	sortGlyphs();

	createRenderBatches();
}


void PlaneColorRenderer::initColorTriangleBatch(size_t mSize)
{
	_triangleGlyphs.resize(mSize);
}

void PlaneColorRenderer::initColorQuadBatch(size_t mSize)
{
	_glyphs.resize(mSize);
}

void PlaneColorRenderer::initColorBoxBatch(size_t mSize)
{
	_boxGlyphs.reserve(mSize);
}

void PlaneColorRenderer::drawTriangle(
	size_t v_index,
	const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3,
	float depth, const Color& color
) {
	_triangleGlyphs[v_index] = TriangleColorGlyph(v1, v2, v3, depth, color);
}
// we can generalize the renderer for multiple kinds of meshes (triangle made instead of planes) by creating
// more draw functions for those meshes (like draw function for triangle).
// Also instead of glyphs have triangles, so when its time to createRenderBatches we see the next mesh
// how many triangles it has and accordingly add those multiple vertices with the combined texture
//! draws are needed to convert the pos and size to vertices
void PlaneColorRenderer::draw(size_t v_index, const glm::vec4& destRect, float depth, const Color& color) {
	_glyphs[v_index] = ColorGlyph(destRect, depth, color);
}

void PlaneColorRenderer::drawBox(const glm::vec4& destRect, float depth, const Color& color) {
	_boxGlyphs.emplace_back(destRect, depth, color);
}

void PlaneColorRenderer::renderBatch(GLSLProgram* glsl_program) {
	glBindVertexArray(_vao);

	GLint centerPosLocation = glGetUniformLocation(glsl_program->getProgramID(), "centerPosition");

	for (int i = 0; i < _renderBatches.size(); i++) {
		glUniform3fv(centerPosLocation, 1, glm::value_ptr(_renderBatches[i].centerPos));

		glDrawArrays(GL_TRIANGLES, _renderBatches[i].offset, _renderBatches[i].numVertices);
	}

	glBindVertexArray(0);
}

void PlaneColorRenderer::createRenderBatches() {

	std::vector<ColorVertex> vertices;

	vertices.resize((_glyphPointers.size() * RECT_OFFSET) + (_triangleGlyphPointers.size() * TRIANGLE_OFFSET));
	
	size_t totalGlyphs = _glyphPointers.size() + _triangleGlyphPointers.size();

	_renderBatches.resize(totalGlyphs);

	if (totalGlyphs == 0) {
		return;
	}

	int offset = 0;
	int cv = 0;



	//current vertex

	/*_threader->parallel(totalGlyphs, [&](int start, int end) {
		int offset = 0;
		int cv = 0;

		for (int i = start; i < end; i++) {
			int cv = 0, offset = 0;

			if (i < _glyphPointers.size()) {

				auto& glyph = _glyphPointers[i];

				offset = i * RECT_OFFSET;
				cv = offset;

				_renderBatches[i] = ColorRenderBatch(offset, RECT_OFFSET, glm::vec3(
					(glyph->topLeft.position.x + glyph->bottomRight.position.x) / 2,
					(glyph->topLeft.position.y + glyph->bottomRight.position.y) / 2,
					(glyph->topLeft.position.z + glyph->bottomRight.position.z) / 2
				));

				vertices[cv++] = glyph->topLeft;
				vertices[cv++] = glyph->bottomLeft;
				vertices[cv++] = glyph->bottomRight;
				vertices[cv++] = glyph->bottomRight;
				vertices[cv++] = glyph->topRight;
				vertices[cv++] = glyph->topLeft;

			}
			else {
				int triangleIdx = i - _glyphPointers.size();

				auto& glyph = _triangleGlyphPointers[triangleIdx];
				
				offset = (_glyphPointers.size() * RECT_OFFSET) + (triangleIdx * TRIANGLE_OFFSET);
				cv = offset;

				_renderBatches[i] = ColorRenderBatch(
					offset, TRIANGLE_OFFSET,
					glm::vec3(glyph->topLeft.position.x, glyph->topLeft.position.y, glyph->topLeft.position.z));

				vertices[cv++] = glyph->topLeft;
				vertices[cv++] = glyph->bottomLeft;
				vertices[cv++] = glyph->bottomRight;
			}

		}
	});*/

	if (_glyphPointers.size())
	{
		_renderBatches[0] = ColorRenderBatch(offset, RECT_OFFSET, glm::vec3(
			(_glyphPointers[0]->topLeft.position.x + _glyphPointers[0]->bottomRight.position.x) / 2,
			(_glyphPointers[0]->topLeft.position.y + _glyphPointers[0]->bottomRight.position.y) / 2,
			(_glyphPointers[0]->topLeft.position.z + _glyphPointers[0]->bottomRight.position.z) / 2
		));
		vertices[cv++] = _glyphPointers[0]->topLeft;
		vertices[cv++] = _glyphPointers[0]->bottomLeft;
		vertices[cv++] = _glyphPointers[0]->bottomRight;
		vertices[cv++] = _glyphPointers[0]->bottomRight;
		vertices[cv++] = _glyphPointers[0]->topRight;
		vertices[cv++] = _glyphPointers[0]->topLeft;
		offset += RECT_OFFSET;

		for (int cg = 1; cg < _glyphPointers.size(); cg++) { //current Glyph
			_renderBatches[cg] = ColorRenderBatch(offset, RECT_OFFSET, glm::vec3(
				(_glyphPointers[cg]->topLeft.position.x + _glyphPointers[cg]->bottomRight.position.x) / 2,
				(_glyphPointers[cg]->topLeft.position.y + _glyphPointers[cg]->bottomRight.position.y) / 2,
				(_glyphPointers[cg]->topLeft.position.z + _glyphPointers[cg]->bottomRight.position.z) / 2
			));
			
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
		_renderBatches[_glyphPointers.size()] = ColorRenderBatch(offset, TRIANGLE_OFFSET, glm::vec3(_triangleGlyphPointers[0]->topLeft.position.x, _triangleGlyphPointers[0]->topLeft.position.y, _triangleGlyphPointers[0]->topLeft.position.z));
		vertices[cv++] = _triangleGlyphPointers[0]->topLeft;
		vertices[cv++] = _triangleGlyphPointers[0]->bottomLeft;
		vertices[cv++] = _triangleGlyphPointers[0]->bottomRight;
		offset += TRIANGLE_OFFSET;

		for (int cg = 1; cg < _triangleGlyphPointers.size(); cg++) { //current Glyph
			_renderBatches[_glyphPointers.size() + cg] =
				ColorRenderBatch(
					offset, 
					TRIANGLE_OFFSET, 
					glm::vec3(_triangleGlyphPointers[cg]->topLeft.position.x, _triangleGlyphPointers[cg]->topLeft.position.y, _triangleGlyphPointers[cg]->topLeft.position.z));

			vertices[cv++] = _triangleGlyphPointers[cg]->topLeft;
			vertices[cv++] = _triangleGlyphPointers[cg]->bottomLeft;
			vertices[cv++] = _triangleGlyphPointers[cg]->bottomRight;
			offset += TRIANGLE_OFFSET;
		}
	}

	// todo not complete
	/*if (_boxGlyphPointers.size()) {
	
	}*/

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	//orphan the buffer / like using double buffer
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ColorVertex), nullptr, GL_DYNAMIC_DRAW);
	//upload the data
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(ColorVertex), vertices.data());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PlaneColorRenderer::createVertexArray() {

	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);

	glBindVertexArray(_vao);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

	glEnableVertexAttribArray(0); // give positions ( point to 0 element for position)
	//position attribute pointer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ColorVertex), (void*)offsetof(ColorVertex, position)); // tell what data it is (first 0) and where the data is ( last 0 to go from the beggining)
	glEnableVertexAttribArray(1);
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
	return (a->topLeft.position.z < b->topLeft.position.z);
}
bool PlaneColorRenderer::compareBackToFront(ColorGlyph* a, ColorGlyph* b) {
	return (a->topLeft.position.z > b->topLeft.position.z);
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
