#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

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

class PlaneColorRenderer : public PlaneRenderer {
public:
	PlaneColorRenderer();
	~PlaneColorRenderer();

	void init();

	void begin();
	void end();

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

	std::vector<Position> sphereVertices = {
		// Generated vertices will go here
	};

	std::vector<GLuint> sphereIndices = {
		// Generated indices will go here
	};

private:
	void createInstancesVBO();
	void createVertexArray();

	std::vector<ColorMeshRenderer> _meshesArrays;
	std::vector<ColorMeshRenderer> _meshesElements;

};