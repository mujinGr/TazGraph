#pragma once

#include <GL/glew.h>
#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/glm.hpp>
#include <vector>

#include "../../../Vertex.h"
#include "../../../GLSLProgram.h"
#include "../PlaneRenderer.h"

// init --_
//		 `-->begin() 
//		 |
//		 | --> draw()
//		 | --> draw()
//		 |
//		 |--> end() 
//		 `--> renderBatch() 

class PlaneModelRenderer : public PlaneRenderer {
public:
	PlaneModelRenderer();
	~PlaneModelRenderer();

	void init();

	void begin();
	void end();

	void initBatchSize();

	void drawTriangle(
		size_t v_index,
		const glm::vec3& triangleOffset,
		const glm::vec3& mRotation,
		const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3,
		GLuint texture);

	void draw(size_t v_index,
		const glm::vec2& rectSize,
		const glm::vec3& bodyCenter,
		const glm::vec3& mRotation,
		const glm::vec4& uvRect,
		GLuint texture
		);

	void renderBatch(GLSLProgram* glsl_program);

	void dispose();
private:
	void createInstancesVBO();
	void createVertexArray();
	
	std::vector<TextureMeshRenderer> _meshesElements;

};
