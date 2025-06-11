#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#include "../../GLSLProgram.h"


// init --_
//		 `-->begin() 
//		 |
//		 | --> draw()
//		 | --> draw()
//		 |
//		 |--> end() 
//		 `--> renderBatch() 

class LightRenderer {
public:
	LightRenderer();
	~LightRenderer();

	void init();

	void begin();
	void end();

	void initLightTriangleBatch(size_t mSize);
	void initLightQuadBatch(size_t mSize);
	void initLightBoxBatch(size_t mSize);
	void initLightSphereBatch(size_t mSize);

	void initBatchSize();

	void drawTriangle(size_t v_index,
		const glm::vec3& depth,
		const glm::vec3& cpuRotation, const Color& color);

	void draw(size_t v_index,
		const glm::vec2& rectSize,
		const glm::vec3& bodyCenter,
		const glm::vec3& mRotation,
		const Color& color);

	void drawBox(size_t v_index,
		const glm::vec3& boxSize,
		const glm::vec3& bodyCenter,
		const glm::vec3& mRotation,
		const Color& color);

	void drawSphere(size_t v_index,
		const glm::vec3& sphereSize,
		const glm::vec3& bodyCenter,
		const glm::vec3& mRotation,
		const Color& color);


	void renderBatch(GLSLProgram* glsl_program);

	void dispose();

private:
	void createRenderBatches();
	void createInstancesVBO();
	void createVertexArray();

	GLuint _vboInstances;

	size_t _glyphs_size = 0; //actual glyphs
	size_t _triangleGlyphs_size = 0; //actual glyphs
	size_t _boxGlyphs_size = 0;
	size_t _sphereGlyphs_size = 0;

	std::vector<ColorMeshRenderer> _meshesArrays;
	std::vector<ColorMeshRenderer> _meshesElements;

};