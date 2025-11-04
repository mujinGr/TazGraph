#pragma once

#include "../../pch.h"

#include "../Vertex.h"
#include "../GLSLProgram.h"

// init --_
//		 `-->begin() 
//		 |
//		 | --> draw()
//		 | --> draw()
//		 |
//		 |--> end() 
//		 `--> renderBatch() 

namespace Taz
{
	class Renderer {
	public:
		Renderer();
		~Renderer();

		virtual void init();

		virtual void begin();
		virtual void end();

		void initBatchSize2(size_t mSize);

		void initLineBatch(size_t mSize);
		void initTriangleBatch(size_t mSize);
		void initQuadBatch(size_t mSize);
		void initBoxBatch(size_t mSize);
		void initSphereBatch(size_t mSize);


		GLuint _vboInstances;

		size_t batchTotalGlyphs = 0;

		size_t _lineGlyphs_size = 0; //actual glyphs
		size_t _triangleGlyphs_size = 0; //actual glyphs
		size_t _rectangleGlyphs_size = 0; //actual glyphs
		size_t _boxGlyphs_size = 0;
		size_t _sphereGlyphs_size = 0;

		virtual void renderBatch() = 0;
	};

}
