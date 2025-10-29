#include "Framebuffer.h"

Framebuffer::Framebuffer() {

}

void Framebuffer::init(int windowWidth, int windowHeight) {
	_width = windowWidth;
	_height = windowHeight;

	glGenVertexArrays(1, &_rectVAO);
	glGenBuffers(1, &_rectVBO);
	glBindVertexArray(_rectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, _rectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(_rectangleVertices), &_rectangleVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


	int samples = 4; // Number of MSAA samples (2, 4, 8, or 16)

	// Create multisampled framebuffer
	glGenFramebuffers(1, &_multisampledFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _multisampledFBO);

	// Create multisampled texture
	glGenTextures(1, &_multisampledTexture);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _multisampledTexture);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, windowWidth, windowHeight, GL_TRUE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, _multisampledTexture, 0);

	// Create multisampled renderbuffer for depth and stencil
	glGenRenderbuffers(1, &_multisampledRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, _multisampledRBO);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _multisampledRBO);

	// Check if multisampled framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR: Multisampled Framebuffer is not complete!" << std::endl;

	// Create regular framebuffer for resolving (this is what you'll actually read from)
	glGenFramebuffers(1, &_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _FBO);

	// Create regular texture
	glGenTextures(1, &_framebufferTexture);
	glBindTexture(GL_TEXTURE_2D, _framebufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _framebufferTexture, 0);

	// Create regular renderbuffer
	glGenRenderbuffers(1, &_RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, _RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _RBO);
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &_FBO);
	glDeleteFramebuffers(1, &_multisampledFBO);
	glDeleteTextures(1, &_framebufferTexture);
	glDeleteTextures(1, &_multisampledTexture);
	glDeleteRenderbuffers(1, &_RBO);
	glDeleteRenderbuffers(1, &_multisampledRBO);
	glDeleteVertexArrays(1, &_rectVAO);
	glDeleteBuffers(1, &_rectVBO);
}

void Framebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _multisampledFBO);
}

void Framebuffer::Unbind()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _multisampledFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _FBO);
	glBlitFramebuffer(0, 0, _width, _height, 0, 0, _width, _height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	// Unbind to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
