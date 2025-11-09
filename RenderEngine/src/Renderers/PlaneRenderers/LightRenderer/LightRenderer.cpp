#include "LightRenderer.h"
#include <algorithm>

LightRenderer::LightRenderer() {

}

LightRenderer::~LightRenderer() {

}

void LightRenderer::init() {
	createVertexArray();

	_meshesArrays[TRIANGLE_MESH_IDX].meshIndices = TRIANGLE_VERTICES;

	_meshesElements[RECTANGLE_MESH_IDX].meshIndices = QUAD_INDICES;
	_meshesElements[BOX_MESH_IDX].meshIndices = ARRAY_BOX_OFFSET;
	_meshesElements[SPHERE_MESH_IDX].meshIndices = sphereIndices.size();
}

void LightRenderer::begin() {
	Renderer::begin();

	for (auto& mesh : _meshesArrays) {
		mesh.batches.clear();
	}
	for (auto& mesh : _meshesElements) {
		mesh.batches.clear();
	}

}
void LightRenderer::end() {
	renderBatch();
}

void LightRenderer::initBatch(const Taz::RenderBatch& batch)
{

	auto initMeshBatch = [&](auto& mesh) {
		mesh.batches.emplace_back();
		mesh.batches.back().instances.resize(batch.count);
		};

	switch (batch.mesh_type) {
	case Taz::RenderBatch::MeshType::Line:
		initMeshBatch(_meshesArrays[TRIANGLE_MESH_IDX]);
		currentBatchIndex = _meshesArrays[TRIANGLE_MESH_IDX].batches.size() - 1;
		break;

	case Taz::RenderBatch::MeshType::Quad:
		initMeshBatch(_meshesElements[RECTANGLE_MESH_IDX]);
		currentBatchIndex = _meshesElements[RECTANGLE_MESH_IDX].batches.size() - 1;

		break;

	case Taz::RenderBatch::MeshType::Box:
		initMeshBatch(_meshesElements[BOX_MESH_IDX]);
		currentBatchIndex = _meshesElements[BOX_MESH_IDX].batches.size() - 1;

		break;

	case Taz::RenderBatch::MeshType::Sphere:
		initMeshBatch(_meshesElements[SPHERE_MESH_IDX]);
		currentBatchIndex = _meshesElements[SPHERE_MESH_IDX].batches.size() - 1;
		break;
	}

}


void LightRenderer::drawTriangle(
	size_t v_index,
	const glm::vec3& position,
	const glm::vec3& cpuRotation,
	const TazColor& color
) {
	glm::vec2 size = glm::vec2(10.0f);
	_meshesArrays[TRIANGLE_MESH_IDX].batches[currentBatchIndex].instances[v_index] = ColorInstanceData(size, position, cpuRotation, color);
}

// we can generalize the renderer for multiple kinds of meshes (triangle made instead of planes) by creating
// more draw functions for those meshes (like draw function for triangle).
// Also instead of glyphs have triangles, so when its time to createRenderBatches we see the next mesh
// how many triangles it has and accordingly add those multiple vertices with the combined texture
//! draws are needed to convert the pos and size to vertices
void LightRenderer::draw(
	size_t v_index,
	const glm::vec2& rectSize,
	const glm::vec3& position,
	const glm::vec3& mRotation,
	const TazColor& color) {
	_meshesElements[RECTANGLE_MESH_IDX].batches[currentBatchIndex].instances[v_index] = ColorInstanceData(rectSize, position, mRotation, color);
}

void LightRenderer::drawBox(
	size_t v_index,
	const glm::vec3& boxSize,
	const glm::vec3& position,
	const glm::vec3& mRotation,
	const TazColor& color) {
	_meshesElements[BOX_MESH_IDX].batches[currentBatchIndex].instances[v_index] = ColorInstanceData(boxSize, position, mRotation, color);
}

void LightRenderer::drawSphere(
	size_t v_index,
	const glm::vec3& sphereSize,
	const glm::vec3& position,
	const glm::vec3& mRotation,
	const TazColor& color)
{
	_meshesElements[SPHERE_MESH_IDX].batches[currentBatchIndex].instances[v_index] = ColorInstanceData(sphereSize, position, mRotation, color);
}

void LightRenderer::renderBatch() {

	for (auto& mesh : _meshesElements) { // different batch for each geometry
		for (auto& batch : mesh.batches) {
			if (batch.instances.empty()) continue;

			glBindVertexArray(mesh.vao);

			glBindBuffer(GL_ARRAY_BUFFER, _vboInstances);

			glBufferData(GL_ARRAY_BUFFER, batch.instances.size() * sizeof(ColorInstanceData), nullptr, GL_DYNAMIC_DRAW);

			glBufferSubData(GL_ARRAY_BUFFER, 0,
				batch.instances.size() * sizeof(ColorInstanceData),
				batch.instances.data());

			glBindBuffer(GL_ARRAY_BUFFER, 0);


			glDrawElementsInstanced(
				GL_TRIANGLES,
				mesh.meshIndices,
				GL_UNSIGNED_INT,
				0,
				batch.instances.size()
			);
		}
	}

	for (auto& mesh : _meshesArrays) { // different batch for each geometry
		for (auto& batch : mesh.batches) {
			if (batch.instances.empty()) continue;

			glBindVertexArray(mesh.vao);

			glBindBuffer(GL_ARRAY_BUFFER, _vboInstances);

			glBufferData(GL_ARRAY_BUFFER, batch.instances.size() * sizeof(ColorInstanceData), nullptr, GL_DYNAMIC_DRAW);

			glBufferSubData(GL_ARRAY_BUFFER, 0,
				batch.instances.size() * sizeof(ColorInstanceData),
				batch.instances.data());


			glDrawArraysInstanced(
				GL_TRIANGLES,
				0,
				mesh.meshIndices,
				batch.instances.size()
			);
		}
	}


	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void LightRenderer::createInstancesVBO() {
	glBindBuffer(GL_ARRAY_BUFFER, _vboInstances);

	glEnableVertexAttribArray(2); // instance size
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ColorInstanceData), (void*)offsetof(ColorInstanceData, size));
	glVertexAttribDivisor(2, 1);

	glEnableVertexAttribArray(3); // instance center
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ColorInstanceData), (void*)offsetof(ColorInstanceData, position));
	glVertexAttribDivisor(3, 1);

	glEnableVertexAttribArray(4); // instance rotation
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(ColorInstanceData), (void*)offsetof(ColorInstanceData, rotation));
	glVertexAttribDivisor(4, 1);

	glEnableVertexAttribArray(5); // instance color
	glVertexAttribPointer(5, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ColorInstanceData), (void*)offsetof(ColorInstanceData, color));
	glVertexAttribDivisor(5, 1);

}

void LightRenderer::createVertexArray() {

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

	//!RECTANGLE STATICS
	glBindVertexArray(_meshesElements[RECTANGLE_MESH_IDX].vao);

	glBindBuffer(GL_ARRAY_BUFFER, _meshesElements[RECTANGLE_MESH_IDX].vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // aPos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TazLightVertex), (void*)offsetof(TazLightVertex, position));

	glEnableVertexAttribArray(1); // aNormal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TazLightVertex), (void*)offsetof(TazLightVertex, normal));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshesElements[RECTANGLE_MESH_IDX].ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

	//!BOX STATICS
	glBindVertexArray(_meshesElements[BOX_MESH_IDX].vao);

	glBindBuffer(GL_ARRAY_BUFFER, _meshesElements[BOX_MESH_IDX].vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(light_cubeVertices), light_cubeVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // aPos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TazLightVertex), (void*)offsetof(TazLightVertex, position));

	glEnableVertexAttribArray(1); // aNormal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TazLightVertex), (void*)offsetof(TazLightVertex, normal));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshesElements[BOX_MESH_IDX].ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

	//!SPHERE STATICS
	glBindVertexArray(_meshesElements[SPHERE_MESH_IDX].vao);

	glBindBuffer(GL_ARRAY_BUFFER, _meshesElements[SPHERE_MESH_IDX].vbo);
	glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(TazLightVertex), sphereVertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // aPos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TazLightVertex), (void*)offsetof(TazLightVertex, position));

	glEnableVertexAttribArray(1); // aNormal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TazLightVertex), (void*)offsetof(TazLightVertex, normal));


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshesElements[SPHERE_MESH_IDX].ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(GLuint), sphereIndices.data(), GL_STATIC_DRAW);



	// triangles/meshesArrays
	glBindVertexArray(_meshesArrays[TRIANGLE_MESH_IDX].vao);

	glBindBuffer(GL_ARRAY_BUFFER, _meshesArrays[TRIANGLE_MESH_IDX].vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // aPos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TazLightVertex), (void*)offsetof(TazLightVertex, position));

	glEnableVertexAttribArray(1); // aNormal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TazLightVertex), (void*)offsetof(TazLightVertex, normal));


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

void LightRenderer::dispose()
{

	for (auto& mesh : _meshesArrays) {
		glDeleteVertexArrays(1, &mesh.vao);
	}

	for (auto& mesh : _meshesElements) {
		glDeleteVertexArrays(1, &mesh.vao);
	}

	for (auto& mesh : _meshesArrays) {
		glDeleteBuffers(1, &mesh.vbo);
		glDeleteBuffers(1, &mesh.ibo);
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
