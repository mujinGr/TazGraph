#include "LineRenderer.h"
#include <algorithm>

constexpr float PI = 3.14159265358f;

LineRenderer::LineRenderer()
{
}

LineRenderer::~LineRenderer()
{
	dispose();
}

void LineRenderer::init()
{
	createVertexArray();
}

void LineRenderer::begin()
{
	Taz::Renderer::begin();

	for (auto& mesh : _meshesArrays) {
		for (auto& meshBatch : mesh.instancesBatches) {
			meshBatch.clear();
		}
	}
	for (auto& mesh : _meshesElements) {
		for (auto& meshBatch : mesh.instancesBatches) {
			meshBatch.clear();
		}
	}
}

void LineRenderer::end() // on en d clear all indices reserved
{
	Taz::Renderer::end();
}


void LineRenderer::initBatchSize()
{
	for (auto& mesh : _meshesArrays) {
		mesh.instancesBatches.emplace_back();
	}

	currentBatchIndex = _meshesArrays[RECTANGLE_MESH_IDX].instancesBatches.size() - 1;

	auto& lineBatch = _meshesArrays[LINE_MESH_IDX].instancesBatches.back();
	auto& rectBatch = _meshesArrays[RECTANGLE_MESH_IDX].instancesBatches.back();
	auto& boxBatch = _meshesArrays[BOX_MESH_IDX].instancesBatches.back();
	auto& sphereBatch = _meshesArrays[SPHERE_MESH_IDX].instancesBatches.back();

	lineBatch.resize(_lineGlyphs_size);
	rectBatch.resize(0);
	boxBatch.resize(0);
	sphereBatch.resize(0);

	for (auto& mesh : _meshesElements) {
		mesh.instancesBatches.emplace_back();
	}
	auto& lineElemBatch = _meshesElements[LINE_MESH_IDX].instancesBatches.back();
	auto& rectElemBatch = _meshesElements[RECTANGLE_MESH_IDX].instancesBatches.back();
	auto& boxElemBatch = _meshesElements[BOX_MESH_IDX].instancesBatches.back();
	auto& sphereElemBatch = _meshesElements[SPHERE_MESH_IDX].instancesBatches.back();

	lineElemBatch.resize(0);
	rectElemBatch.resize(0);
	boxElemBatch.resize(0);
	sphereElemBatch.resize(0);

	_meshesArrays[LINE_MESH_IDX].meshIndices = INDICES_LINE_OFFSET;
	_meshesArrays[RECTANGLE_MESH_IDX].meshIndices = QUAD_INDICES;
	_meshesArrays[BOX_MESH_IDX].meshIndices = INDICES_BOX_OFFSET;

	_meshesElements[LINE_MESH_IDX].meshIndices = INDICES_LINE_OFFSET;
	_meshesElements[RECTANGLE_MESH_IDX].meshIndices = QUAD_INDICES;
	_meshesElements[BOX_MESH_IDX].meshIndices = INDICES_BOX_OFFSET;
}

// todo can be optimized, by having something like glyphs in planeModelRenederer where first you pass info in a vector and
// todo on render pass that info in verts and indices
void LineRenderer::drawLine(size_t v_index, const glm::vec3 srcPosition, const glm::vec3 destPosition, const TazColor& srcColor, const TazColor& destColor, const float width)
{
	_meshesArrays[LINE_MESH_IDX].instancesBatches[currentBatchIndex][v_index] = LineInstanceData(srcPosition, destPosition, srcColor, destColor, width);
}

void LineRenderer::drawRectangle(size_t v_index, const glm::vec2& rectSize,
	const glm::vec3& position,
	const TazColor& color,
	const glm::vec3& mRotation,
	const float width)
{
	_meshesElements[LINE_RECTANGLE_MESH_IDX].instancesBatches[currentBatchIndex][v_index] = WireframeInstanceData(rectSize, position, mRotation, color, width);
}

void LineRenderer::drawBox(size_t v_index, const glm::vec3& rectSize,
	const glm::vec3& position,
	const TazColor& color,
	const glm::vec3& mRotation,
	const float width)
{
	_meshesElements[LINE_BOX_MESH_IDX].instancesBatches[currentBatchIndex][v_index] = WireframeInstanceData(rectSize, position, mRotation, color, width);

}
void LineRenderer::drawCircle(const glm::vec2& center, const TazColor& color, float radius)
{
}


void LineRenderer::renderBatch()
{
	for (auto& mesh : _meshesElements) { // different batch for each geometry
		for (auto& batch : mesh.instancesBatches) {
			if (batch.empty()) continue;

			glBindVertexArray(mesh.vao);

			glBindBuffer(GL_ARRAY_BUFFER, _vboInstances);

			glBufferData(GL_ARRAY_BUFFER
				, batch.size() * sizeof(LineInstanceData),
				batch.data(),
				GL_DYNAMIC_DRAW);

			glBufferSubData(GL_ARRAY_BUFFER, 0,
				batch.size() * sizeof(LineInstanceData),
				batch.data());

			glBindBuffer(GL_ARRAY_BUFFER, 0);


			glDrawArraysInstanced(
				GL_LINES,
				0,
				2,
				static_cast<GLsizei>(batch.size())
			);
		}
	}

	renderElementsBatch();
}

void LineRenderer::renderElementsBatch() {
	for (auto& mesh : _meshesElements) { // different batch for each geometry
		for (auto& batch : mesh.instancesBatches) {
			if (batch.empty()) continue;

			glBindVertexArray(mesh.vao);

			glBindBuffer(GL_ARRAY_BUFFER, _vboInstances);

			glBufferData(GL_ARRAY_BUFFER
				, batch.size() * sizeof(WireframeInstanceData),
				batch.data(),
				GL_DYNAMIC_DRAW);

			glBufferSubData(GL_ARRAY_BUFFER, 0,
				batch.size() * sizeof(WireframeInstanceData),
				batch.data());

			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawElementsInstanced(
				GL_LINES,
				mesh.meshIndices,
				GL_UNSIGNED_INT,
				0,
				batch.size()
			);
		}
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void LineRenderer::createInstancesVBO() {
	glBindBuffer(GL_ARRAY_BUFFER, _vboInstances);

	glEnableVertexAttribArray(0); // instance fromPos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineInstanceData), (void*)offsetof(LineInstanceData, fromPos));
	glVertexAttribDivisor(0, 1);

	glEnableVertexAttribArray(1); // instance toPos
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LineInstanceData), (void*)offsetof(LineInstanceData, toPos));
	glVertexAttribDivisor(1, 1);

	glEnableVertexAttribArray(2); // instance fromColor
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(LineInstanceData), (void*)offsetof(LineInstanceData, fromColor));
	glVertexAttribDivisor(2, 1);

	glEnableVertexAttribArray(3); // instance toColor
	glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(LineInstanceData), (void*)offsetof(LineInstanceData, toColor));
	glVertexAttribDivisor(3, 1);

	glEnableVertexAttribArray(4); // instance width
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(LineInstanceData), (void*)offsetof(LineInstanceData, width));
	glVertexAttribDivisor(4, 1);
}

void LineRenderer::createWireframeInstancesVBO() {
	glBindBuffer(GL_ARRAY_BUFFER, _vboInstances);

	glEnableVertexAttribArray(1); // instance TazSize
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(WireframeInstanceData), (void*)offsetof(WireframeInstanceData, size));
	glVertexAttribDivisor(1, 1);

	glEnableVertexAttribArray(2); // instance Body Center
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(WireframeInstanceData), (void*)offsetof(WireframeInstanceData, position));
	glVertexAttribDivisor(2, 1);

	glEnableVertexAttribArray(3); // instance TazColor
	glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(WireframeInstanceData), (void*)offsetof(WireframeInstanceData, color));
	glVertexAttribDivisor(3, 1);

	glEnableVertexAttribArray(4); // instance TazRotation
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(WireframeInstanceData), (void*)offsetof(WireframeInstanceData, rotation));
	glVertexAttribDivisor(4, 1);

	glEnableVertexAttribArray(5); // instance Width
	glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(WireframeInstanceData), (void*)offsetof(WireframeInstanceData, width));
	glVertexAttribDivisor(5, 1);
}

void LineRenderer::createVertexArray() {
	_meshesArrays.resize(TOTAL_MESHES);
	_meshesElements.resize(TOTAL_MESHES);

	for (int i = 0; i < _meshesArrays.size(); i++) {
		glGenVertexArrays(1, &_meshesArrays[i].vao);
		glGenBuffers(1, &_meshesArrays[i].vbo);
		glGenBuffers(1, &_meshesArrays[i].ibo);
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TazPosition), (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshesElements[RECTANGLE_MESH_IDX].ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadWireframeIndices), quadWireframeIndices, GL_STATIC_DRAW);

	//!BOX STATICS
	glBindVertexArray(_meshesElements[BOX_MESH_IDX].vao);

	glBindBuffer(GL_ARRAY_BUFFER, _meshesElements[BOX_MESH_IDX].vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // aPos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TazPosition), (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshesElements[BOX_MESH_IDX].ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeWireframeIndices), cubeWireframeIndices, GL_STATIC_DRAW);

	////////////////////////////////////

	glGenBuffers(1, &_vboInstances);

	for (int i = 0; i < _meshesArrays.size(); i++) {

		glBindVertexArray(_meshesArrays[i].vao);

		createInstancesVBO();
	}

	for (int i = 0; i < _meshesElements.size(); i++) {

		glBindVertexArray(_meshesElements[i].vao);

		createWireframeInstancesVBO();
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void LineRenderer::dispose()
{
	for (auto& mesh : _meshesArrays) {
		glDeleteVertexArrays(1, &mesh.vao);
	}

	for (auto& mesh : _meshesArrays) {
		glDeleteBuffers(1, &mesh.vbo);
		glDeleteBuffers(1, &mesh.ibo);
	}
	for (auto& mesh : _meshesArrays) {
		glDeleteVertexArrays(1, &mesh.vao);
	}

	for (auto& mesh : _meshesArrays) {
		glDeleteBuffers(1, &mesh.vbo);
		glDeleteBuffers(1, &mesh.ibo);
	}

	if (_vboInstances) {
		glDeleteBuffers(1, &_vboInstances);
	}
}
