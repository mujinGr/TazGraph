#include "PlaneModelRenderer.h"
#include <algorithm>

PlaneModelRenderer::PlaneModelRenderer() : _vboInstances(0),
_glyphs_size(0)
{

}

PlaneModelRenderer::~PlaneModelRenderer() {

}

void PlaneModelRenderer::init() {
	createVertexArray();
}

void PlaneModelRenderer::begin() {
	_glyphs_size = 0;

	_vertices.clear();
	_indices.clear();

	for (auto& mesh : _meshesElements) {
		mesh.instances.clear();
	}
}
void PlaneModelRenderer::end() {
	//set up all pointers for fast sorting
	createRenderBatches();
}


void PlaneModelRenderer::initTextureQuadBatch(size_t mSize)
{
	_glyphs_size = mSize;
}

void PlaneModelRenderer::initBatchSize()
{
	_vertices.resize((_glyphs_size * SQUARE_OFFSET));
	_indices.resize(_glyphs_size * QUAD_INDICES);

	_meshesElements[TRIANGLE_MESH_IDX].instances.resize(0);

	_meshesElements[RECTANGLE_MESH_IDX].instances.resize(_glyphs_size);
	_meshesElements[RECTANGLE_MESH_IDX].meshIndices = QUAD_INDICES;

	_meshesElements[BOX_MESH_IDX].instances.resize(0);
}

void PlaneModelRenderer::drawTriangle(
	size_t v_index,
	const glm::vec3& triangleOffset,
	const glm::vec3& mRotation,
	const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3,
	GLuint texture
) {

}
// we can generalize the renderer for multiple kinds of meshes (triangle made instead of planes) by creating
// more draw functions for those meshes (like draw function for triangle).
// Also instead of glyphs have triangles, so when its time to createRenderBatches we see the next mesh
// how many triangles it has and accordingly add those multiple vertices with the combined texture
void PlaneModelRenderer::draw(
	size_t v_index,
	const glm::vec2& rectSize,
	const glm::vec3& bodyCenter,
	const glm::vec3& mRotation,
	const glm::vec4& uvRect,
	GLuint texture
) {

	Glyph glyph = Glyph(rectSize, mRotation, uvRect, texture, bodyCenter.z);

	int offset = v_index * SQUARE_OFFSET;
	int ind_offset = v_index * QUAD_INDICES;

	for (size_t i = 0; i < QUAD_INDICES; i++) {
		_indices[ind_offset + i] = offset + quadIndices[i];
	}

	_vertices[offset++] = glyph.topLeft;
	_vertices[offset++] = glyph.bottomLeft;
	_vertices[offset++] = glyph.bottomRight;
	_vertices[offset++] = glyph.topRight;

	_meshesElements[RECTANGLE_MESH_IDX].instances[v_index] = TextureInstanceData(rectSize, bodyCenter, mRotation, texture);
}

void PlaneModelRenderer::renderBatch(GLSLProgram* glsl_program) {

	if (_meshesElements[RECTANGLE_MESH_IDX].instances.size() == 0) return;

	glBindVertexArray(_meshesElements[RECTANGLE_MESH_IDX].vao);

	GLint centerPosLocation = glGetUniformLocation(glsl_program->getProgramID(), "centerPosition");

	for (int i = 0; i < _meshesElements[RECTANGLE_MESH_IDX].instances.size(); i++) {
		glUniform3fv(centerPosLocation, 1, glm::value_ptr(_meshesElements[RECTANGLE_MESH_IDX].instances[i].bodyCenter));
		
		glBindTexture(GL_TEXTURE_2D, _meshesElements[RECTANGLE_MESH_IDX].instances[i].texture);

		glDrawElements(GL_TRIANGLES, _meshesElements[RECTANGLE_MESH_IDX].meshIndices, GL_UNSIGNED_INT, (void*)(0));
	}

	glBindVertexArray(0);
}

void PlaneModelRenderer::createRenderBatches() {

	if (_glyphs_size == 0 ) {
		return;
	}

	glBindBuffer(GL_ARRAY_BUFFER, _meshesElements[RECTANGLE_MESH_IDX].vbo);
	//orphan the buffer / like using double buffer
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(TextureVertex), nullptr, GL_DYNAMIC_DRAW);
	//upload the data
	glBufferSubData(GL_ARRAY_BUFFER, 0, _vertices.size() * sizeof(TextureVertex), _vertices.data());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshesElements[RECTANGLE_MESH_IDX].ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), _indices.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PlaneModelRenderer::createInstancesVBO() {
	//glBindBuffer(GL_ARRAY_BUFFER, _vboInstances);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), (void*)offsetof(TextureVertex, position)); // tell what data it is (first 0) and where the data is ( last 0 to go from the beggining)
	//glVertexAttribDivisor(1, 1);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), (void*)offsetof(TextureVertex, rotation));
	//glVertexAttribDivisor(2, 1);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), (void*)offsetof(TextureVertex, uv));
	//glVertexAttribDivisor(3, 1);

	//glEnableVertexAttribArray(4); // instance uv
	//glVertexAttribPointer(4, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ColorInstanceData), (void*)offsetof(ColorInstanceData, color));
	//glVertexAttribDivisor(4, 1);


	//glEnableVertexAttribArray(4); // instance texture
	//glVertexAttribPointer(4, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ColorInstanceData), (void*)offsetof(ColorInstanceData, color));
	//glVertexAttribDivisor(4, 1);

}


void PlaneModelRenderer::createVertexArray() {

	_meshesElements.resize(TOTAL_MESHES);

	for (int i = 0; i < _meshesElements.size(); i++) {
		glGenVertexArrays(1, &_meshesElements[i].vao);
		glGenBuffers(1, &_meshesElements[i].vbo);
		glGenBuffers(1, &_meshesElements[i].ibo);

	}

	for (int i = 0; i < _meshesElements.size(); i++) {

		glBindVertexArray(_meshesElements[i].vao);

		glBindBuffer(GL_ARRAY_BUFFER, _meshesElements[i].vbo);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshesElements[RECTANGLE_MESH_IDX].ibo);
		createInstancesVBO();
	}


	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void PlaneModelRenderer::dispose()
{
	for (auto& mesh : _meshesElements) {
		glDeleteVertexArrays(1, &mesh.vao);
	}

	for (auto& mesh : _meshesElements) {
		glDeleteBuffers(1, &mesh.vbo);
		glDeleteBuffers(1, &mesh.ibo);
	}

	if (_vboInstances) {
		glDeleteBuffers(1, &_vboInstances);
	}
	//_program.dispose();
}
