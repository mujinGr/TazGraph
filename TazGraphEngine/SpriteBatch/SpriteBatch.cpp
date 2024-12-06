#include "SpriteBatch.h"
#include <algorithm>

SpriteBatch::SpriteBatch() : _vbo(0), _vao(0) {

}

SpriteBatch::~SpriteBatch() {

}

void SpriteBatch::init() {
	createVertexArray();
}

void SpriteBatch::begin(GlyphSortType sortType/*GlyphSortType::TEXTURE*/) {
	_sortType = sortType;
	_renderBatches.clear();

	_glyphs.clear();
}
void SpriteBatch::end() {
	//set up all pointers for fast sorting
	_glyphPointers.resize(_glyphs.size());
	for (int i = 0; i < _glyphs.size(); i++) {
		_glyphPointers[i] = &_glyphs[i];
	}
	sortGlyphs();
	createRenderBatches();
}

void SpriteBatch::draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const Color& color, float angle) {
	_glyphs.emplace_back(destRect, uvRect, texture, depth, color, angle);
}

void SpriteBatch::renderBatch() {
	glBindVertexArray(_vao);

	for (int i = 0; i < _renderBatches.size(); i++) {
		glBindTexture(GL_TEXTURE_2D, _renderBatches[i].texture);

		glDrawArrays(GL_TRIANGLES, _renderBatches[i].offset, _renderBatches[i].numVertices);
	}

	glBindVertexArray(0);
}

void SpriteBatch::createRenderBatches() {
	std::vector<Vertex> vertices;
	vertices.resize(_glyphPointers.size() * 6);
	if (_glyphPointers.empty()) {
		return;
	}

	int offset = 0;
	int cv = 0; //current vertex
	_renderBatches.emplace_back(offset, 6, _glyphPointers[0]->texture);
	vertices[cv++] = _glyphPointers[0]->topLeft;
	vertices[cv++] = _glyphPointers[0]->bottomLeft;
	vertices[cv++] = _glyphPointers[0]->bottomRight;
	vertices[cv++] = _glyphPointers[0]->bottomRight;
	vertices[cv++] = _glyphPointers[0]->topRight;
	vertices[cv++] = _glyphPointers[0]->topLeft;
	offset += 6;

	for (int cg = 1; cg < _glyphPointers.size(); cg++) { //current Glyph
		if (_glyphPointers[cg]->texture != _glyphPointers[cg - 1]->texture) {
			_renderBatches.emplace_back(offset, 6, _glyphPointers[0]->texture);
		}
		else {
			_renderBatches.back().numVertices += 6;
		}
		vertices[cv++] = _glyphPointers[cg]->topLeft;
		vertices[cv++] = _glyphPointers[cg]->bottomLeft;
		vertices[cv++] = _glyphPointers[cg]->bottomRight;
		vertices[cv++] = _glyphPointers[cg]->bottomRight;
		vertices[cv++] = _glyphPointers[cg]->topRight;
		vertices[cv++] = _glyphPointers[cg]->topLeft;
		offset += 6;
	}

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	//orphan the buffer
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
	//upload the data
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SpriteBatch::createVertexArray() {

	if (_vao == 0) {
		glGenVertexArrays(1, &_vao);
	}
	glBindVertexArray(_vao);

	if (_vbo == 0) {
		glGenBuffers(1, &_vbo);
	}
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

void SpriteBatch::sortGlyphs() {
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

bool SpriteBatch::compareFrontToBack(Glyph* a, Glyph* b) {
	return (a->depth < b->depth);
}
bool SpriteBatch::compareBackToFront(Glyph* a, Glyph* b) {
	return (a->depth > b->depth);
}
bool SpriteBatch::compareTexture(Glyph* a, Glyph* b) {
	return (a->texture < b->texture);
}