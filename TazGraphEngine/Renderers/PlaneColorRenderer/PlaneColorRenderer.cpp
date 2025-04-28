#include "PlaneColorRenderer.h"
#include <algorithm>

PlaneColorRenderer::PlaneColorRenderer() : 
	_quadVbo(0),
	_triangleVbo(0),
	_quadIbo(0),
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
	_meshesArrays[TRIANGLE_MESH_IDX].offset = 0;
	_meshesElements[RECTANGLE_MESH_IDX].offset = 0;

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
	const glm::vec3& bodyCenter, const glm::vec3& cpuRotation, 
	const Color& color
) {
	glm::vec2 size = glm::vec2(10.0f);
	_meshesArrays[TRIANGLE_MESH_IDX].instances[v_index] = InstanceData(size, bodyCenter, cpuRotation, color);
}

// we can generalize the renderer for multiple kinds of meshes (triangle made instead of planes) by creating
// more draw functions for those meshes (like draw function for triangle).
// Also instead of glyphs have triangles, so when its time to createRenderBatches we see the next mesh
// how many triangles it has and accordingly add those multiple vertices with the combined texture
//! draws are needed to convert the pos and size to vertices
void PlaneColorRenderer::draw(size_t v_index, const glm::vec2& rectSize, const glm::vec3& bodyCenter, const glm::vec3& mRotation, const Color& color) {
	_meshesElements[RECTANGLE_MESH_IDX].instances[v_index] = InstanceData(rectSize, bodyCenter, mRotation,color);
}

void PlaneColorRenderer::drawBox(const glm::vec4& destRect, float depth, const Color& color) {
	//_boxGlyphs.emplace_back(destRect, depth, color);
}

void PlaneColorRenderer::renderBatch(GLSLProgram* glsl_program) {

	for (int i = 0; i < _meshesElements.size(); i++) { // different batch for each geometry
		
		if (_meshesElements[i].instances.size() == 0) continue;
		
		glBindVertexArray(_meshesElements[i].vao);

		glBindBuffer(GL_ARRAY_BUFFER, _vboInstances);

		glBufferData(GL_ARRAY_BUFFER, _meshesElements[i].instances.size() * sizeof(InstanceData), nullptr, GL_DYNAMIC_DRAW);

		glBufferSubData(GL_ARRAY_BUFFER, 0,
			_meshesElements[i].instances.size() * sizeof(InstanceData),
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

		glBufferData(GL_ARRAY_BUFFER, _meshesArrays[i].instances.size() * sizeof(InstanceData), nullptr, GL_DYNAMIC_DRAW);

		glBufferSubData(GL_ARRAY_BUFFER, 0,
			_meshesArrays[i].instances.size() * sizeof(InstanceData),
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

void PlaneColorRenderer::createVertexArray() {

	_meshesArrays.resize(TOTAL_MESHES);
	_meshesElements.resize(TOTAL_MESHES);

	glGenVertexArrays(1, &_meshesArrays[TRIANGLE_MESH_IDX].vao);
	glGenVertexArrays(1, &_meshesElements[RECTANGLE_MESH_IDX].vao);

	glGenBuffers(1, &_quadVbo);
	glGenBuffers(1, &_triangleVbo);
	glGenBuffers(1, &_vboInstances);
	glGenBuffers(1, &_quadIbo);

	glBindVertexArray(_meshesElements[RECTANGLE_MESH_IDX].vao);

	glBindBuffer(GL_ARRAY_BUFFER, _quadVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0); // aPos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Position), (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _quadIbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Position), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, _vboInstances);

	glEnableVertexAttribArray(1); // instance size
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, size));
	glVertexAttribDivisor(1, 1);

	glEnableVertexAttribArray(2); // instance center
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, bodyCenter));
	glVertexAttribDivisor(2, 1);

	glEnableVertexAttribArray(3); // instance rotation
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, rotation));
	glVertexAttribDivisor(3, 1);

	glEnableVertexAttribArray(4); // instance color
	glVertexAttribPointer(4, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(InstanceData), (void*)offsetof(InstanceData, color));
	glVertexAttribDivisor(4, 1);


	glBindVertexArray(_meshesArrays[TRIANGLE_MESH_IDX].vao);

	glBindBuffer(GL_ARRAY_BUFFER, _triangleVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Position), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, _vboInstances);

	glEnableVertexAttribArray(1); // instance size
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, size));
	glVertexAttribDivisor(1, 1);

	glEnableVertexAttribArray(2); // instance center
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, bodyCenter));
	glVertexAttribDivisor(2, 1);

	glEnableVertexAttribArray(3); // instance center
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, rotation));
	glVertexAttribDivisor(3, 1);

	glEnableVertexAttribArray(4); // instance color
	glVertexAttribPointer(4, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(InstanceData), (void*)offsetof(InstanceData, color));
	glVertexAttribDivisor(4, 1);

	
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

	if (_quadVbo) {
		glDeleteBuffers(1, &_quadVbo);
	}
	if (_triangleVbo) {
		glDeleteBuffers(1, &_triangleVbo);
	}
	if (_vboInstances) {
		glDeleteBuffers(1, &_vboInstances);
	}
	if (_quadIbo) {
		glDeleteBuffers(1, &_quadIbo);
	}
	//_program.dispose();
}
