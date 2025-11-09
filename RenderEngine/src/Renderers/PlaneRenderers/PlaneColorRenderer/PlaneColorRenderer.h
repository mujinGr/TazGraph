#pragma once

#include "../../../GLSLProgram.h"
#include "../../Renderer.h"

// init --_
//		 `-->begin() 
//		 |
//		 | --> draw()
//		 | --> draw()
//		 |
//		 |--> end() 
//		 `--> renderBatch() 

class PlaneColorRenderer : public Taz::Renderer {
public:
	PlaneColorRenderer();
	~PlaneColorRenderer();

	void init();

	void begin();
	void end();

	void initBatch(Taz::RenderBatch& batch);

	void drawTriangle(size_t v_index,
		const glm::vec3& depth,
		const glm::vec3& cpuRotation, const TazColor& color);

	void draw(size_t v_index,
		const glm::vec2& rectSize,
		const glm::vec3& position,
		const glm::vec3& mRotation,
		const TazColor& color);

	void drawBox(size_t v_index,
		const glm::vec3& boxSize,
		const glm::vec3& position,
		const glm::vec3& mRotation,
		const TazColor& color);

	void drawSphere(size_t v_index,
		const glm::vec3& sphereSize,
		const glm::vec3& position,
		const glm::vec3& mRotation,
		const TazColor& color);

	void endBatch(const Taz::RenderBatch& batch) override;

	void renderBatch() override;

	void dispose();

	std::vector<TazPosition> sphereVertices = {
		// Generated vertices will go here
	};

	std::vector<GLuint> sphereIndices = {
		// Generated indices will go here
	};

private:
	void createInstancesVBO();
	void createVertexArray();

	std::vector<ColorMeshRenderer> _meshesArrays;
	std::vector<ColorMeshRenderer> _meshesElements;

};