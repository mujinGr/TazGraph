#pragma once

#include <GL/glew.h>
#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/glm.hpp>
#include <vector>

#include "../../Vertex.h"
#include "../../GLSLProgram.h"

class RenderBatch {
public:
	RenderBatch(GLuint Offset, GLuint NumIndices, glm::vec3 CenterPos, GLuint Texture) : offset(Offset),
		numIndices(NumIndices),
		centerPos(CenterPos),
		texture(Texture) {

	}
	GLuint offset;
	GLuint numIndices;
	glm::vec3 centerPos = glm::vec3(0);
	GLuint texture;
};


class Glyph {

public:
	Glyph() {};
	Glyph(const glm::vec2& rectSize,
		const glm::vec3& mRotation,
		const glm::vec4& uvRect,
		GLuint texture, float Depth)
		: texture(texture) {

		float halfW = rectSize.x / 2.0f;
		float halfH = rectSize.y / 2.0f;

		Position positions[4] = {
	   {-halfW, -halfH, 0.0f},  // topLeft
	   {-halfW,  halfH, 0.0f},  // bottomLeft
	   { halfW,  halfH, 0.0f},  // bottomRight
	   { halfW, -halfH, 0.0f}   // topRight
		};

		glm::vec2 uv_topLeft = glm::vec2(uvRect.x, uvRect.y);
		glm::vec2 uv_bottomLeft = glm::vec2(uvRect.x, uvRect.y + uvRect.w);
		glm::vec2 uv_bottomRight = glm::vec2(uvRect.x + uvRect.z, uvRect.y + uvRect.w);
		glm::vec2 uv_topRight = glm::vec2(uvRect.x + uvRect.z, uvRect.y);
			
		topLeft.setPosition(positions[0]);
		topLeft.rotation = mRotation;
		topLeft.setUV(uv_topLeft); // Use bottom y for top

		bottomLeft.setPosition(positions[1]);
		bottomLeft.rotation = mRotation;
		bottomLeft.setUV(uv_bottomLeft); // Use top y for bottom

		bottomRight.setPosition(positions[2]);
		bottomRight.rotation = mRotation;
		bottomRight.setUV(uv_bottomRight); // Use top y for bottom

		topRight.setPosition(positions[3]);
		topRight.rotation = mRotation;
		topRight.setUV(uv_topRight); // Use bottom y for top
	};

	GLuint texture = 0;

	TextureVertex topLeft;
	TextureVertex bottomLeft;
	TextureVertex topRight;
	TextureVertex bottomRight;
};

// init --_
//		 `-->begin() 
//		 |
//		 | --> draw()
//		 | --> draw()
//		 |
//		 |--> end() 
//		 `--> renderBatch() 

class PlaneModelRenderer {
public:
	PlaneModelRenderer();
	~PlaneModelRenderer();

	void init();

	void begin();
	void end();

	void initTextureQuadBatch(size_t mSize);

	void initBatchSize();

	void drawTriangle(
		size_t v_index,
		const glm::vec3& triangleOffset,
		const glm::vec3& mRotation,
		const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3,
		GLuint texture);

	void draw(size_t v_index,
		const glm::vec2& rectSize,
		const glm::vec3& bodyCenter,
		const glm::vec3& mRotation,
		const glm::vec4& uvRect,
		GLuint texture
		);

	void renderBatch(GLSLProgram* glsl_program);

	void dispose();
private:
	void createRenderBatches();
	void createInstancesVBO();
	void createVertexArray();
	
	GLuint _vboInstances;

	std::vector<TextureVertex> _vertices;
	std::vector<GLuint> _indices;

	size_t _glyphs_size = 0; //actual glyphs

	std::vector<TextureMeshRenderer> _meshesElements;

};
