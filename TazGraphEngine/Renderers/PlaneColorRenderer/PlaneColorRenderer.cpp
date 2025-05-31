#include "PlaneColorRenderer.h"
#include <algorithm>

PlaneColorRenderer::PlaneColorRenderer() : 
	_vboInstances(0),

	_glyphs_size(0),
	_triangleGlyphs_size(0),
	_boxGlyphs_size(0){

}

PlaneColorRenderer::~PlaneColorRenderer() {

}

void PlaneColorRenderer::init() {
	createVertexArray();
}

void PlaneColorRenderer::begin() {

	for (auto& mesh : _meshesArrays) {
		mesh.instances.clear();
	}
	for (auto& mesh : _meshesElements) {
		mesh.instances.clear();
	}

	_glyphs_size = 0;
	_triangleGlyphs_size = 0;
	_boxGlyphs_size = 0;
}
void PlaneColorRenderer::end() {

	//set up all pointers for fast sorting
	createRenderBatches();
}

void PlaneColorRenderer::initColorQuadBatch(size_t mSize)
{
	_glyphs_size = mSize;
}

void PlaneColorRenderer::initColorTriangleBatch(size_t mSize)
{
	_triangleGlyphs_size = mSize;
}

void PlaneColorRenderer::initColorBoxBatch(size_t mSize)
{
	_boxGlyphs_size = mSize;
}

void PlaneColorRenderer::initBatchSize()
{
	_meshesArrays[TRIANGLE_MESH_IDX].instances.resize(_triangleGlyphs_size);
	_meshesArrays[TRIANGLE_MESH_IDX].meshIndices = TRIANGLE_VERTICES;
	_meshesArrays[RECTANGLE_MESH_IDX].instances.resize(0);
	_meshesArrays[BOX_MESH_IDX].instances.resize(0);

	_meshesElements[TRIANGLE_MESH_IDX].instances.resize(0);
	_meshesElements[RECTANGLE_MESH_IDX].instances.resize(_glyphs_size);
	_meshesElements[RECTANGLE_MESH_IDX].meshIndices = QUAD_INDICES;
	_meshesElements[BOX_MESH_IDX].instances.resize(0);
}


void PlaneColorRenderer::drawTriangle(
	size_t v_index,
	const glm::vec3& bodyCenter,
	const glm::vec3& cpuRotation, 
	const Color& color
) {
	glm::vec2 size = glm::vec2(10.0f);
	_meshesArrays[TRIANGLE_MESH_IDX].instances[v_index] = ColorInstanceData(size, bodyCenter, cpuRotation, color);
}

// we can generalize the renderer for multiple kinds of meshes (triangle made instead of planes) by creating
// more draw functions for those meshes (like draw function for triangle).
// Also instead of glyphs have triangles, so when its time to createRenderBatches we see the next mesh
// how many triangles it has and accordingly add those multiple vertices with the combined texture
//! draws are needed to convert the pos and size to vertices
void PlaneColorRenderer::draw(
	size_t v_index,
	const glm::vec2& rectSize,
	const glm::vec3& bodyCenter,
	const glm::vec3& mRotation,
	const Color& color) {
	_meshesElements[RECTANGLE_MESH_IDX].instances[v_index] = ColorInstanceData(rectSize, bodyCenter, mRotation, color);
}

void PlaneColorRenderer::drawBox(
	size_t v_index,
	const glm::vec3& boxSize,
	const glm::vec3& bodyCenter,
	const glm::vec3& mRotation,
	const Color& color) {
	_meshesElements[BOX_MESH_IDX].instances[v_index] = ColorInstanceData(boxSize, bodyCenter, mRotation, color);

	//_boxGlyphs.emplace_back(destRect, depth, color);
}

void PlaneColorRenderer::renderBatch(GLSLProgram* glsl_program) {

	for (int i = 0; i < _meshesElements.size(); i++) { // different batch for each geometry
		
		if (_meshesElements[i].instances.size() == 0) continue;
		
		glBindVertexArray(_meshesElements[i].vao);

		glBindBuffer(GL_ARRAY_BUFFER, _vboInstances);

		glBufferData(GL_ARRAY_BUFFER, _meshesElements[i].instances.size() * sizeof(ColorInstanceData), nullptr, GL_DYNAMIC_DRAW);

		glBufferSubData(GL_ARRAY_BUFFER, 0,
			_meshesElements[i].instances.size() * sizeof(ColorInstanceData),
			_meshesElements[i].instances.data());

		glBindBuffer(GL_ARRAY_BUFFER, 0);


		glDrawElementsInstanced(
			GL_TRIANGLES,
			_meshesElements[i].meshIndices,
			GL_UNSIGNED_INT,
			0,
			_meshesElements[i].instances.size()
		);
	}


	for (int i = 0; i < _meshesArrays.size(); i++) {
		
		if (_meshesArrays[i].instances.size() == 0) continue;

		glBindVertexArray(_meshesArrays[i].vao);

		glBindBuffer(GL_ARRAY_BUFFER, _vboInstances);

		glBufferData(GL_ARRAY_BUFFER, _meshesArrays[i].instances.size() * sizeof(ColorInstanceData), nullptr, GL_DYNAMIC_DRAW);

		glBufferSubData(GL_ARRAY_BUFFER, 0,
			_meshesArrays[i].instances.size() * sizeof(ColorInstanceData),
			_meshesArrays[i].instances.data());


		glDrawArraysInstanced(
			GL_TRIANGLES,
			0,
			_meshesArrays[i].meshIndices,
			_meshesArrays[i].instances.size()
		);
	}


	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PlaneColorRenderer::createRenderBatches() {

	//current vertex
	if ((_glyphs_size + _triangleGlyphs_size) == 0) {
		return;
	}

}

void PlaneColorRenderer::createInstancesVBO() {
	glBindBuffer(GL_ARRAY_BUFFER, _vboInstances);

	glEnableVertexAttribArray(1); // instance size
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ColorInstanceData), (void*)offsetof(ColorInstanceData, size));
	glVertexAttribDivisor(1, 1);

	glEnableVertexAttribArray(2); // instance center
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ColorInstanceData), (void*)offsetof(ColorInstanceData, bodyCenter));
	glVertexAttribDivisor(2, 1);

	glEnableVertexAttribArray(3); // instance rotation
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ColorInstanceData), (void*)offsetof(ColorInstanceData, rotation));
	glVertexAttribDivisor(3, 1);

	glEnableVertexAttribArray(4); // instance color
	glVertexAttribPointer(4, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ColorInstanceData), (void*)offsetof(ColorInstanceData, color));
	glVertexAttribDivisor(4, 1);

}

void PlaneColorRenderer::createVertexArray() {

	_meshesArrays.resize(TOTAL_MESHES);
	_meshesElements.resize(TOTAL_MESHES);

	for (int i = 0; i < _meshesArrays.size(); i++) {
		glGenVertexArrays(1, &_meshesArrays[i].vao);
		glGenBuffers(1, &_meshesElements[i].vbo);

	}

	for (int i = 0; i < _meshesElements.size(); i++) {
		glGenVertexArrays(1, &_meshesElements[i].vao);
		glGenBuffers(1, &_meshesElements[i].vbo);
		glGenBuffers(1, &_meshesElements[i].ibo);

	}

	glGenBuffers(1, &_vboInstances);
	
	for (int i = 0; i < _meshesElements.size(); i++) {

		glBindVertexArray(_meshesElements[i].vao);

		createInstancesVBO();
	}

	for (int i = 0; i < _meshesArrays.size(); i++) {

		glBindVertexArray(_meshesArrays[i].vao);

		createInstancesVBO();
	}

	glBindVertexArray(_meshesElements[RECTANGLE_MESH_IDX].vao);

	glBindBuffer(GL_ARRAY_BUFFER, _meshesElements[RECTANGLE_MESH_IDX].vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0); // aPos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Position), (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshesElements[RECTANGLE_MESH_IDX].ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Position), (void*)0);


	glBindVertexArray(_meshesArrays[TRIANGLE_MESH_IDX].vao);

	glBindBuffer(GL_ARRAY_BUFFER, _meshesArrays[TRIANGLE_MESH_IDX].vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Position), (void*)0);
	
	//unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void PlaneColorRenderer::dispose()
{

	for (auto& mesh : _meshesArrays) {
		glDeleteVertexArrays(1, &mesh.vao);
	}

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
