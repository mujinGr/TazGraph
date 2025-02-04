#include "Framebuffer.h"

Framebuffer::Framebuffer() {

}

void Framebuffer::init(int windowWidth, int windowHeight) {
	glGenVertexArrays(1, &_rectVAO);
	glGenBuffers(1, &_rectVBO);
	glBindVertexArray(_rectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, _rectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(_rectangleVertices), &_rectangleVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


	//! invalidate()
	// Create frame buffer object
	glGenFramebuffers(1, &_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _FBO);

	// Create Framebuffer Texture
	glGenTextures(1, &_framebufferTexture);
	glBindTexture(GL_TEXTURE_2D, _framebufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _framebufferTexture, 0);

	// Create Render Buffer Object
	glGenRenderbuffers(1, &_RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, _RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _RBO);

}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &_FBO);
}

void Framebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
}

void Framebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
