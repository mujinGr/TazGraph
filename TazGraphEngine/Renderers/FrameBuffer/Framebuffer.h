#pragma once

#include "../../GLSLProgram.h"
#include <GL/glew.h>
#include <glm/glm.hpp>

class Framebuffer
{
private:
	float _rectangleVertices[24] =
	{
		// Coords    // texCoords
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,

		 1.0f,  1.0f,  1.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f
	};

	unsigned int _rectVAO, _rectVBO;

	unsigned int _FBO;
	unsigned int _RBO;
public:
	unsigned int _framebufferTexture;

	Framebuffer();
	~Framebuffer();

	void init(int windowWidth, int windowHeight);

	void Bind();
	void Unbind();
};