#include "./AppInterface.h"


void AppInterface::drawBatch(const std::vector<Entity*>& entities, LineRenderer& batch) {

	threadPool.parallel(entities.size(), [&](int start, int end) {
		for (int i = start; i < end; i++) {
			entities[i]->draw(i, batch, _window);
		}
		});

}

void AppInterface::drawBatch(const std::vector<Entity*>& entities, PlaneColorRenderer& batch) {

	threadPool.parallel(entities.size(), [&](int start, int end) {
		for (int i = start; i < end; i++) {
			entities[i]->draw(i, batch, _window);
		}
		});
}

void AppInterface::drawBatch(const std::vector<Entity*>& entities, PlaneModelRenderer& batch) {
	// before calling this make sure that reserved the right amount of memory

	for (int i = 0; i < entities.size(); i++) {
		entities[i]->draw(i, batch, _window);
	}

}

void AppInterface::drawBatch(const std::vector<Entity*>& entities, LightRenderer& batch) {
	// before calling this make sure that reserved the right amount of memory

	for (int i = 0; i < entities.size(); i++) {
		entities[i]->draw(i, batch, _window);
	}
}

void AppInterface::prepareBatch(
	Taz::GECSRenderBatch& batch)
{
	switch (batch.renderer_type) {
	case Taz::RenderBatch::RendererType::Line:
		prepareLineBatch(batch);
		break;
	case Taz::RenderBatch::RendererType::PlaneColor:
		preparePlaneColorBatch(batch);
		break;
	case Taz::RenderBatch::RendererType::PlaneModel:
		preparePlaneModelBatch(batch);
		break;
	case Taz::RenderBatch::RendererType::Light:
		prepareLightBatch(batch);
		break;
	}
}

void AppInterface::prepareLineBatch(
	Taz::GECSRenderBatch& batch
)
{
	//lineRenderer.initBatch(batch);
	lineRenderer.initBatch(batch);

	drawBatch(batch.entities, lineRenderer);
}

void AppInterface::preparePlaneColorBatch(
	Taz::GECSRenderBatch& batch)
{
	planeColorRenderer.initBatch(batch);

	// Fill batch data
	drawBatch(batch.entities, planeColorRenderer);
}

void AppInterface::preparePlaneModelBatch(
	Taz::GECSRenderBatch& batch)
{
	planeModelRenderer.initBatch(batch);

	// Fill batch data
	drawBatch(batch.entities, planeModelRenderer);
}

void AppInterface::prepareLightBatch(
	Taz::GECSRenderBatch& batch
)
{
	lightRenderer.initBatch(batch);

	// Fill batch data
	drawBatch(batch.entities, lightRenderer);
}


void AppInterface::renderBatch(
	const Taz::GECSRenderBatch& batch,
	const Taz::FrameRenderData& frameData,
	ICamera& camera)
{
	if (batch.count == 0) return;

	switch (batch.renderer_type) {
	case Taz::RenderBatch::RendererType::Line:
		drawLineBatch(batch, frameData, camera);
		break;
	case Taz::RenderBatch::RendererType::PlaneColor:
		drawPlaneColorBatch(batch, frameData, camera);
		break;
	case Taz::RenderBatch::RendererType::PlaneModel:
		drawPlaneModelBatch(batch, frameData, camera);
		break;
	case Taz::RenderBatch::RendererType::Light:
		drawLightBatch(batch, frameData, camera);
		break;
	}
}

void AppInterface::drawLineBatch(
	const Taz::GECSRenderBatch& batch,
	const Taz::FrameRenderData& frameData,
	ICamera& camera
)
{
	// Setup shader and render
	auto& shader = *resourceManager.getGLSLProgram(batch.shaderName);
	resourceManager.setupShader(shader, camera);

	if (batch.viewportSize != glm::vec2(0.0f)) {
		GLint pLocation = shader.getUniformLocation("viewportSize");
		glUniform2f(pLocation, batch.viewportSize.x, batch.viewportSize.y);
	}

	lineRenderer.endBatch(batch);
	shader.unuse();

}

void AppInterface::drawPlaneColorBatch(
	const Taz::GECSRenderBatch& batch,
	const Taz::FrameRenderData& frameData,
	ICamera& camera)
{
	// Setup shader and render
	auto& shader = *resourceManager.getGLSLProgram(batch.shaderName);
	resourceManager.setupShader(shader, camera);

	GLint pLocation = shader.getUniformLocation("rotationMatrix");
	glUniformMatrix4fv(pLocation, 1, GL_FALSE, glm::value_ptr(batch.rotationMatrix));

	planeColorRenderer.endBatch(batch);
	shader.unuse();

}

void AppInterface::drawPlaneModelBatch(
	const Taz::GECSRenderBatch& batch,
	const Taz::FrameRenderData& frameData,
	ICamera& camera)
{
	// Setup shader and render
	auto& shader = *resourceManager.getGLSLProgram(batch.shaderName);
	resourceManager.setupShader(shader, camera);

	planeModelRenderer.endBatch(batch);
	shader.unuse();

}

void AppInterface::drawLightBatch(
	const Taz::GECSRenderBatch& batch,
	const Taz::FrameRenderData& frameData,
	ICamera& camera
)
{
	// Setup shader and render
	auto& shader = *resourceManager.getGLSLProgram(batch.shaderName);
	resourceManager.setupShader(shader, camera);

	lightRenderer.endBatch(batch);
	shader.unuse();

}


