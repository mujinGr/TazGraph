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
		enum class RendererType {
			Line,
			PlaneColor,
			PlaneModel,
			Light
		};

		RendererType renderer_type = RendererType::Line;

		enum class MeshType {
			Line,
			Triangle,
			Quad,
			Box,
			Sphere
		};
		MeshType mesh_type = MeshType::Quad;

		std::string shaderName;

		size_t count = 0;

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

		GLuint _vboInstances;

		size_t _lineGlyphs_size = 0; //actual glyphs
		size_t _triangleGlyphs_size = 0; //actual glyphs
		size_t _rectangleGlyphs_size = 0; //actual glyphs
		size_t _boxGlyphs_size = 0;
		size_t _sphereGlyphs_size = 0;

		virtual void renderBatch() = 0;
	};

}
