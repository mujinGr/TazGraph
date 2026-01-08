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
	bool _multisampleEnabled = false;
	int _multisamples = 0;

	static void SetMultisample(bool enable) {
		if (enable) {
			glEnable(GL_MULTISAMPLE);
		}
		else {
			glDisable(GL_MULTISAMPLE);
		}
	}

	uint32_t _framebufferTexture;
	uint32_t _multisampledTexture;

	int _width, _height;

	Framebuffer();
	~Framebuffer();

	void init(int windowWidth, int windowHeight, bool enableMSAA, int MSAA_samples = 0);

	void setMultisampleFramebufferSize(int windowWidth, int windowHeight);

	void setSize(int windowWidth, int windowHeight);

	void Bind();
	void Unbind();
};