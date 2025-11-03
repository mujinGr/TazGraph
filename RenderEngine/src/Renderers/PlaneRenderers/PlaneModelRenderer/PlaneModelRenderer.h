#pragma once

#include "../../../Vertex.h"
#include "../../../GLSLProgram.h"
#include "../../Renderer.h"

// init --_
//		 `-->begin() 
//		 |
//		 | --> draw()
//		 | --> draw()
//		 |
//		 |--> end() 
//		 `--> renderBatch() 

class PlaneModelRenderer : public Taz::Renderer {
public:
	PlaneModelRenderer();
	~PlaneModelRenderer();

	void init() override;

	void begin() override;
	void end() override;

	void initBatchSize();

	void drawTriangle(
		size_t v_index,
		const glm::vec3& triangleOffset,
		const glm::vec3& mRotation,
		const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3,
		GLuint texture);

	void draw(size_t v_index,
		const glm::vec2& rectSize,
		const glm::vec3& position,
		const glm::vec3& mRotation,
		const glm::vec4& uvRect,
		GLuint texture
	);

	void renderBatch() override;

	void dispose();
private:
	void createInstancesVBO();
	void createVertexArray();

	std::vector<TextureMeshRenderer> _meshesElements;

	size_t currentBatchIndex = 0;

};
