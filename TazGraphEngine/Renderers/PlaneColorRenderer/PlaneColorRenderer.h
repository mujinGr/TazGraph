#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#include "../../GLSLProgram.h"

struct InstanceData {

	InstanceData() {}
	InstanceData(glm::vec3 mSize, Position mBodyCenter, Rotation mRotation, Color mColor) :
		size(mSize),
		bodyCenter(mBodyCenter),
		rotation(mRotation),
		color(mColor)
	{}
	InstanceData(glm::vec2 mSize, Position mBodyCenter, Rotation mRotation, Color mColor) :
		size(glm::vec3(mSize,0.0f)),
		bodyCenter(mBodyCenter),
		rotation(mRotation),
		color(mColor)
	{
	}
	~InstanceData() {}

	Size size;
	Position bodyCenter;
	Rotation rotation;

	Color color;
};

struct MeshRenderer {
	size_t meshIndices = 0;

	std::vector<InstanceData> instances;

	GLuint vao;

	GLuint vbo;
	GLuint ibo;
};


// init --_
//		 `-->begin() 
//		 |
//		 | --> draw()
//		 | --> draw()
//		 |
//		 |--> end() 
//		 `--> renderBatch() 

class PlaneColorRenderer {
public:
	PlaneColorRenderer();
	~PlaneColorRenderer();

	void init();

	void begin();
	void end();

	void initColorTriangleBatch(size_t mSize);
	void initColorQuadBatch(size_t mSize);
	void initColorBoxBatch(size_t mSize);
	
	void initBatchSize();

	void drawTriangle(size_t v_index,
		const glm::vec3& depth,
		const glm::vec3& cpuRotation, const Color& color);

	void draw(size_t v_index, const glm::vec2& rectSize, const glm::vec3& bodyCenter, const glm::vec3& mRotation, const Color& color);

	void drawBox(const glm::vec4& destRect, float depth, const Color& color);

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

	std::vector<MeshRenderer> _meshesArrays;
	std::vector<MeshRenderer> _meshesElements;

};