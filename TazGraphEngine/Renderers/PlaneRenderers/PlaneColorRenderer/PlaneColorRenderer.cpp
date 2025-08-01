#include "PlaneColorRenderer.h"
#include <algorithm>

PlaneColorRenderer::PlaneColorRenderer(){
}

PlaneColorRenderer::~PlaneColorRenderer() {

}

void PlaneColorRenderer::init() {
	createVertexArray();
}

void PlaneColorRenderer::begin() {

	PlaneRenderer::begin();

	for (auto& mesh : _meshesArrays) {
		mesh.instances.clear();
	}
	for (auto& mesh : _meshesElements) {
		mesh.instances.clear();
	}
}
void PlaneColorRenderer::end() {
	PlaneRenderer::end();
}

void PlaneColorRenderer::initBatchSize()
{
	//mesh Arrays
	_meshesArrays[TRIANGLE_MESH_IDX].instances.resize(_triangleGlyphs_size);
	_meshesArrays[TRIANGLE_MESH_IDX].meshIndices = TRIANGLE_VERTICES;

	_meshesArrays[RECTANGLE_MESH_IDX].instances.resize(0);
	_meshesArrays[BOX_MESH_IDX].instances.resize(0);
	_meshesArrays[SPHERE_MESH_IDX].instances.resize(0);


	//mesh Elements

	_meshesElements[TRIANGLE_MESH_IDX].instances.resize(0);

	_meshesElements[RECTANGLE_MESH_IDX].instances.resize(_glyphs_size);
	_meshesElements[RECTANGLE_MESH_IDX].meshIndices = QUAD_INDICES;
	
	_meshesElements[BOX_MESH_IDX].instances.resize(_boxGlyphs_size);
	_meshesElements[BOX_MESH_IDX].meshIndices = INDICES_BOX_OFFSET;

	_meshesElements[SPHERE_MESH_IDX].instances.resize(_sphereGlyphs_size);
	_meshesElements[SPHERE_MESH_IDX].meshIndices = sphereIndices.size();

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
}

void PlaneColorRenderer::drawSphere(
	size_t v_index,
	const glm::vec3& sphereSize,
	const glm::vec3& bodyCenter,
	const glm::vec3& mRotation,
	const Color& color)
{
	_meshesElements[SPHERE_MESH_IDX].instances[v_index] = ColorInstanceData(sphereSize, bodyCenter, mRotation, color);
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
		glGenBuffers(1, &_meshesArrays[i].vbo);

	}

	for (int i = 0; i < _meshesElements.size(); i++) {
		glGenVertexArrays(1, &_meshesElements[i].vao);
		glGenBuffers(1, &_meshesElements[i].vbo);
		glGenBuffers(1, &_meshesElements[i].ibo);

	}

	//!RECTANGLE STATICS
	glBindVertexArray(_meshesElements[RECTANGLE_MESH_IDX].vao);

	glBindBuffer(GL_ARRAY_BUFFER, _meshesElements[RECTANGLE_MESH_IDX].vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // aPos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Position), (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshesElements[RECTANGLE_MESH_IDX].ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

	//!BOX STATICS
	glBindVertexArray(_meshesElements[BOX_MESH_IDX].vao);

	glBindBuffer(GL_ARRAY_BUFFER, _meshesElements[BOX_MESH_IDX].vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // aPos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Position), (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshesElements[BOX_MESH_IDX].ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

	//!SPHERE STATICS
	glBindVertexArray(_meshesElements[SPHERE_MESH_IDX].vao);

	glBindBuffer(GL_ARRAY_BUFFER, _meshesElements[SPHERE_MESH_IDX].vbo);
	glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(Position), sphereVertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // aPos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Position), (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshesElements[SPHERE_MESH_IDX].ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(GLuint), sphereIndices.data(), GL_STATIC_DRAW);


	// triangles/meshesArrays
	glBindVertexArray(_meshesArrays[TRIANGLE_MESH_IDX].vao);

	glBindBuffer(GL_ARRAY_BUFFER, _meshesArrays[TRIANGLE_MESH_IDX].vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Position), (void*)0);


	glGenBuffers(1, &_vboInstances);
	
	for (int i = 0; i < _meshesElements.size(); i++) {

		glBindVertexArray(_meshesElements[i].vao);

		createInstancesVBO();
	}

	for (int i = 0; i < _meshesArrays.size(); i++) {

		glBindVertexArray(_meshesArrays[i].vao);

		createInstancesVBO();
	}

	// rectangles/meshesElements

	
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
