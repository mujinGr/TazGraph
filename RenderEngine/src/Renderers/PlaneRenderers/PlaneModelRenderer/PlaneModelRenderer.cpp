#include "PlaneModelRenderer.h"
#include <algorithm>

PlaneModelRenderer::PlaneModelRenderer()
{

}

PlaneModelRenderer::~PlaneModelRenderer() {

}

void PlaneModelRenderer::init() {
	createVertexArray();

	_meshesElements[RECTANGLE_MESH_IDX].meshIndices = QUAD_INDICES;
}

void PlaneModelRenderer::begin() {

	Renderer::begin();

	for (auto& mesh : _meshesElements) {
		mesh.batches.clear();
	}

}

void PlaneModelRenderer::initBatch(Taz::RenderBatch& batch)
{
	auto initMeshBatch = [&](auto& mesh) {
		mesh.batches.emplace_back();
		mesh.batches.back().batchName = batch.batchName;
		mesh.batches.back().instances.resize(batch.count);
		};

	switch (batch.mesh_type) {
	case Taz::RenderBatch::MeshType::Quad:
		initMeshBatch(_meshesElements[RECTANGLE_MESH_IDX]);
		currentBatchIndex = _meshesElements[RECTANGLE_MESH_IDX].batches.size() - 1;
		break;
	}
	batch.index = currentBatchIndex;

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
	const glm::vec3& position,
	const glm::vec3& mRotation,
	const glm::vec4& uvRect,
	GLuint texture
) {

	_meshesElements[RECTANGLE_MESH_IDX].batches[currentBatchIndex].instances[v_index] = TextureInstanceData(rectSize, position, mRotation, texture);
}

void PlaneModelRenderer::endBatch(const Taz::RenderBatch& batch) {
	GLuint vao = 0;
	GLuint instanceVBO = _vboInstances;
	size_t instanceCount = 0;
	const void* instanceData = nullptr;
	size_t instanceDataSize = 0;
	GLenum drawMode = GL_TRIANGLES;

	switch (batch.mesh_type) {
	case Taz::RenderBatch::MeshType::Quad: {
		auto& mesh = _meshesElements[RECTANGLE_MESH_IDX];

		if (mesh.batches.empty() || batch.index >= mesh.batches.size())
			return;

		auto& b = mesh.batches[batch.index];
		if (b.instances.empty()) return;

		vao = mesh.vao;
		glBindVertexArray(vao);

		for (auto& instance : b.instances) {
			glBindTexture(GL_TEXTURE_2D, instance.texture);

			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(TextureInstanceData), nullptr, GL_DYNAMIC_DRAW);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TextureInstanceData), &instance);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawElementsInstanced(drawMode, mesh.meshIndices, GL_UNSIGNED_INT, 0, 1);

			glBindTexture(GL_TEXTURE_2D, 0);
		}

		b.instances.clear();
		break;
	}
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PlaneModelRenderer::createInstancesVBO() {
	glBindBuffer(GL_ARRAY_BUFFER, _vboInstances);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TextureInstanceData), (void*)offsetof(TextureInstanceData, size)); // tell what data it is (first 0) and where the data is ( last 0 to go from the beggining)
	glVertexAttribDivisor(0, 1);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TextureInstanceData), (void*)offsetof(TextureInstanceData, position));
	glVertexAttribDivisor(1, 1);

	glEnableVertexAttribArray(2); // instance rotation
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TextureInstanceData), (void*)offsetof(TextureInstanceData, rotation));
	glVertexAttribDivisor(2, 1);

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

	glGenBuffers(1, &_vboInstances);

	for (int i = 0; i < _meshesElements.size(); i++) {

		glBindVertexArray(_meshesElements[i].vao);

		glBindBuffer(GL_ARRAY_BUFFER, _meshesElements[i].vbo);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshesElements[RECTANGLE_MESH_IDX].ibo);
		createInstancesVBO();
	}

	glBindVertexArray(_meshesElements[RECTANGLE_MESH_IDX].vao);

	glBindBuffer(GL_ARRAY_BUFFER, _meshesElements[RECTANGLE_MESH_IDX].vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tex_quadVertices), tex_quadVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(3); // aPos
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(TazTexVertex), (void*)offsetof(TazTexVertex, position));

	glEnableVertexAttribArray(4); // aUV
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(TazTexVertex), (void*)offsetof(TazTexVertex, uv));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshesElements[RECTANGLE_MESH_IDX].ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);


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
