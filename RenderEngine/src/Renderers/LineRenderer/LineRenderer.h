#pragma once

#include "../../GLSLProgram.h"

#include "../Renderer.h"
#include "../../Vertex.h"

class LineRenderer : public Taz::Renderer {
public:
	const char* VERT_SRC = R"(#version 400

in vec3 vertexPosition; //vec3 is array of 3 floats
in vec4 vertexColor;

out vec4 fragmentColor;

uniform mat4 u_ViewProjection;

void main() {
    gl_Position = u_ViewProjection * vec4(vertexPosition.xyz, 1.0);

    fragmentColor = vertexColor;
})";

	const char* FRAG_SRC = R"(#version 400

in vec4 fragmentColor;

out vec4 color; //rgb value

void main() {
    color = vec4(fragmentColor.rgb, fragmentColor.a);
})";

	LineRenderer();
	~LineRenderer();

	void init();

	void begin();
	void end();

	void initBatch(Taz::RenderBatch& batch);

	void drawLine(size_t v_index,
		const glm::vec3 srcPosition, const glm::vec3 destPosition,
		const TazColor& srcColor, const TazColor& destColor,
		const float width = 5.0f);
	void drawRectangle(size_t v_index, const glm::vec2& rectSize,
		const glm::vec3& position,
		const TazColor& color,
		const glm::vec3& mRotation = glm::vec3(0),
		const float width = 5.0f);
	void drawBox(size_t v_index, const glm::vec3& rectSize,
		const glm::vec3& position,
		const TazColor& color,
		const glm::vec3& mRotation = glm::vec3(0),
		const float width = 5.0f);
	void drawCircle(const glm::vec2& center, const TazColor& color, float radius);

	void endBatch(const Taz::RenderBatch& batch);

	void renderBatch() override;

	void renderElementsBatch();

	void dispose();

	int box_edgePairs[12][2] = {
			{0, 1}, {1, 2}, {2, 3}, {3, 0}, // Bottom face
			{4, 5}, {5, 6}, {6, 7}, {7, 4}, // Top face
			{0, 4}, {1, 5}, {2, 6}, {3, 7}  // Vertical edges
	};

private:
	void createInstancesVBO();
	void createWireframeInstancesVBO();
	void createVertexArray();

	std::vector<LineMeshRenderer> _meshesArrays;
	std::vector<WireframeMeshRenderer> _meshesElements;

	size_t currentBatchIndex = 0;

};