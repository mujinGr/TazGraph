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
	struct RenderBatch {
		enum class Type {
			Line,
			PlaneColor,
			PlaneModel,
			Light
		};

		Type type;
		std::string shaderName;

		size_t lineCount = 0;
		size_t quadCount = 0;
		size_t triangleCount = 0;
		size_t boxCount = 0;
		size_t sphereCount = 0;

		// Additional shader uniforms
		glm::mat4 rotationMatrix = glm::mat4(1.0f);
		glm::vec2 viewportSize = glm::vec2(0.0f);
	};



	class Renderer {
	public:
		Renderer();
		~Renderer();

		virtual void init();

		virtual void begin();
		virtual void end() = 0;

		void initLineBatch(size_t mSize);
		void initTriangleBatch(size_t mSize);
		void initQuadBatch(size_t mSize);
		void initBoxBatch(size_t mSize);
		void initSphereBatch(size_t mSize);


		GLuint _vboInstances;

		size_t _lineGlyphs_size = 0; //actual glyphs
		size_t _triangleGlyphs_size = 0; //actual glyphs
		size_t _rectangleGlyphs_size = 0; //actual glyphs
		size_t _boxGlyphs_size = 0;
		size_t _sphereGlyphs_size = 0;

		virtual void renderBatch() = 0;
	};

}
