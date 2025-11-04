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
		virtual void end();

		GLuint _vboInstances;

		virtual void renderBatch() = 0;
	};

}
