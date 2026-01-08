#include "Framebuffer.h"

Framebuffer::Framebuffer() {

}

void Framebuffer::init(int windowWidth, int windowHeight, bool enableMSAA, int MSAA_samples) {

	_multisampleEnabled = enableMSAA;
	_multisamples = MSAA_samples;

	glGenVertexArrays(1, &_rectVAO);
	glGenBuffers(1, &_rectVBO);
	glBindVertexArray(_rectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, _rectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(_rectangleVertices), &_rectangleVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	// Create regular framebuffer for resolving (this is what you'll actually read from)
	if (_multisampleEnabled) {
		glGenFramebuffers(1, &_multisampledFBO);
		glGenRenderbuffers(1, &_multisampledRBO);
		glGenTextures(1, &_multisampledTexture);

	}
	glGenFramebuffers(1, &_FBO);
	glGenRenderbuffers(1, &_RBO);
	glGenTextures(1, &_framebufferTexture);

	setSize(windowWidth, windowHeight);
}

void Framebuffer::setMultisampleFramebufferSize(int windowWidth, int windowHeight) {
	if (_multisampleEnabled) {
		// Create multisampled framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, _multisampledFBO);
		// Create multisampled texture
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _multisampledTexture);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, _multisamples, GL_RGB, windowWidth, windowHeight, GL_TRUE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, _multisampledTexture, 0);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
		// Create multisampled renderbuffer for depth and stencil
		glBindRenderbuffer(GL_RENDERBUFFER, _multisampledRBO);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, _multisamples, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _multisampledRBO);
		// Check if multisampled framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			TAZ_ERROR("Multisampled Framebuffer is not complete!");
	}
}

void Framebuffer::setSize(int windowWidth, int windowHeight) {
	_width = windowWidth;
	_height = windowHeight;

	setMultisampleFramebufferSize(windowWidth, windowHeight);

	glBindFramebuffer(GL_FRAMEBUFFER, _FBO);

	// Create regular texture
	glBindTexture(GL_TEXTURE_2D, _framebufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _framebufferTexture, 0);

	// Create regular renderbuffer
	glBindRenderbuffer(GL_RENDERBUFFER, _RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _RBO);

	Unbind();
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
	if (_multisampleEnabled) {
		glBindFramebuffer(GL_FRAMEBUFFER, _multisampledFBO);
	}
	else {
		glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
	}
}

void Framebuffer::Unbind()
{
	if (_multisampleEnabled) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, _multisampledFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _FBO);
		glBlitFramebuffer(0, 0, _width, _height, 0, 0, _width, _height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}
	// Unbind to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
