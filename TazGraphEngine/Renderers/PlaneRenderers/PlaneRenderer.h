#pragma once

#include <GL/glew.h>
#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/glm.hpp>
#include <vector>

#include "../../Vertex.h"
#include "../../GLSLProgram.h"

// init --_
//		 `-->begin() 
//		 |
//		 | --> draw()
//		 | --> draw()
//		 |
//		 |--> end() 
//		 `--> renderBatch() 

class PlaneRenderer {
public:
	PlaneRenderer();
	~PlaneRenderer();

	void init();

	void begin();
	void end();

	void initTriangleBatch(size_t mSize);
	void initQuadBatch(size_t mSize);
	void initBoxBatch(size_t mSize);
	void initSphereBatch(size_t mSize);


	GLuint _vboInstances;

	size_t _glyphs_size = 0; //actual glyphs
	size_t _triangleGlyphs_size = 0; //actual glyphs
	size_t _boxGlyphs_size = 0;
	size_t _sphereGlyphs_size = 0;
	
	void createRenderBatches();
};
