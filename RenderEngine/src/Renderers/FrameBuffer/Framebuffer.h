#pragma once

#include "../../GLSLProgram.h"

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

	unsigned int _multisampledFBO;
	unsigned int _multisampledRBO;
public:
	uint32_t _framebufferTexture;
	uint32_t _multisampledTexture;

	int _width, _height;

	Framebuffer();
	~Framebuffer();

	void init(int windowWidth, int windowHeight);

	void Bind();
	void Unbind();
};