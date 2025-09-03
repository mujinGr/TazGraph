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
		mesh.instances.clear();
	}
}

void LineRenderer::end() // on en d clear all indices reserved
{
	Taz::Renderer::end();
}


void LineRenderer::initBatchSize()
{
	_meshesArrays[LINE_MESH_IDX].instances.resize(_lineGlyphs_size);
	_meshesArrays[LINE_MESH_IDX].meshIndices = INDICES_LINE_OFFSET;

	_meshesArrays[RECTANGLE_MESH_IDX].instances.resize(0);
	_meshesArrays[RECTANGLE_MESH_IDX].meshIndices = QUAD_INDICES;

	_meshesArrays[BOX_MESH_IDX].instances.resize(0);
	_meshesArrays[BOX_MESH_IDX].meshIndices = INDICES_BOX_OFFSET;

	_meshesArrays[SPHERE_MESH_IDX].instances.resize(0);
}

// todo can be optimized, by having something like glyphs in planeModelRenederer where first you pass info in a vector and
// todo on render pass that info in verts and indices
void LineRenderer::drawLine(size_t v_index, const glm::vec3 srcPosition, const glm::vec3 destPosition, const Color& srcColor, const Color& destColor)
{
	_meshesArrays[LINE_MESH_IDX].instances[v_index] = LineInstanceData(srcPosition, destPosition, srcColor, destColor, 5.0f);
}

void LineRenderer::drawRectangle(size_t v_index, const glm::vec2& rectSize,
	const glm::vec3& bodyCenter,
	const glm::vec3& mRotation,
	const Color& color)
{
	_meshesElements[LINE_RECTANGLE_MESH_IDX].instances[v_index] = WireframeInstanceData(rectSize, bodyCenter, mRotation, color, 5.0f);
}

void LineRenderer::drawBox(size_t v_index, const glm::vec2& rectSize,
	const glm::vec3& bodyCenter,
	const glm::vec3& mRotation,
	const Color& color)
{
	_meshesElements[LINE_BOX_MESH_IDX].instances[v_index] = WireframeInstanceData(rectSize, bodyCenter, mRotation, color, 5.0f);

}
void LineRenderer::drawCircle(const glm::vec2& center, const Color& color, float radius)
{
}


void LineRenderer::renderBatch()
{
	for (int i = 0; i < _meshesArrays.size(); i++) { // different batch for each geometry

		if (_meshesArrays[i].instances.size() == 0) continue;

		glBindVertexArray(_meshesArrays[i].vao);

		glBindBuffer(GL_ARRAY_BUFFER, _vboInstances);

		glBufferData(GL_ARRAY_BUFFER
			, _meshesArrays[i].instances.size() * sizeof(LineInstanceData),
			_meshesArrays[i].instances.data(),
			GL_DYNAMIC_DRAW);

		glBufferSubData(GL_ARRAY_BUFFER, 0,
			_meshesArrays[i].instances.size() * sizeof(LineInstanceData),
			_meshesArrays[i].instances.data());

		glBindBuffer(GL_ARRAY_BUFFER, 0);


		glDrawArraysInstanced(
			GL_LINES,
			0,
			2,
			static_cast<GLsizei>(_meshesArrays[i].instances.size())
		);
	}

	//for (int i = 0; i < _meshesElements.size(); i++) { // different batch for each geometry

	//	if (_meshesElements[i].instances.size() == 0) continue;

	//	glBindVertexArray(_meshesElements[i].vao);

	//	glBindBuffer(GL_ARRAY_BUFFER, _vboInstances);

	//	glBufferData(GL_ARRAY_BUFFER
	//		, _meshesElements[i].instances.size() * sizeof(LineInstanceData),
	//		_meshesElements[i].instances.data(),
	//		GL_DYNAMIC_DRAW);

	//	glBufferSubData(GL_ARRAY_BUFFER, 0,
	//		_meshesElements[i].instances.size() * sizeof(LineInstanceData),
	//		_meshesElements[i].instances.data());

	//	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//	glDrawElementsInstanced(
	//		GL_LINES,
	//		_meshesElements[i].meshIndices,
	//		GL_UNSIGNED_INT,
	//		0,
	//		_meshesElements[i].instances.size()
	//	);
	//}

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

void LineRenderer::createVertexArray() {
	_meshesArrays.resize(TOTAL_MESHES);

	for (int i = 0; i < _meshesArrays.size(); i++) {
		glGenVertexArrays(1, &_meshesArrays[i].vao);
		glGenBuffers(1, &_meshesArrays[i].vbo);
		glGenBuffers(1, &_meshesArrays[i].ibo);
	}

	//!RECTANGLE STATICS
	glBindVertexArray(_meshesElements[RECTANGLE_MESH_IDX].vao);

	glBindBuffer(GL_ARRAY_BUFFER, _meshesElements[RECTANGLE_MESH_IDX].vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // aPos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Position), (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshesElements[RECTANGLE_MESH_IDX].ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadWireframeIndices), quadWireframeIndices, GL_STATIC_DRAW);

	//!BOX STATICS
	glBindVertexArray(_meshesElements[BOX_MESH_IDX].vao);

	glBindBuffer(GL_ARRAY_BUFFER, _meshesElements[BOX_MESH_IDX].vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // aPos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Position), (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshesElements[BOX_MESH_IDX].ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeWireframeIndices), cubeWireframeIndices, GL_STATIC_DRAW);

	////////////////////////////////////

	glGenBuffers(1, &_vboInstances);

	for (int i = 0; i < _meshesArrays.size(); i++) {

		glBindVertexArray(_meshesArrays[i].vao);

		createInstancesVBO();
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


	if (_vboInstances) {
		glDeleteBuffers(1, &_vboInstances);
	}
}
