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
	Taz::GECSRenderBatch& batch,
	Taz::FrameRenderData& frameData
)
{
	switch (batch.renderer_type) {
	case Taz::RenderBatch::RendererType::Line:
		prepareLineBatch(batch, frameData);
		break;
	case Taz::RenderBatch::RendererType::PlaneColor:
		preparePlaneColorBatch(batch, frameData);
		break;
	case Taz::RenderBatch::RendererType::PlaneModel:
		preparePlaneModelBatch(batch, frameData);
		break;
	case Taz::RenderBatch::RendererType::Light:
		prepareLightBatch(batch, frameData);
		break;
	}
}

void AppInterface::prepareLineBatch(
	Taz::GECSRenderBatch& batch,
	Taz::FrameRenderData& frameData
)
{
	//lineRenderer.initBatch(batch);
	frameData.lineRenderer.initBatch(batch);

	drawBatch(batch.entities, frameData.lineRenderer);
}

void AppInterface::preparePlaneColorBatch(
	Taz::GECSRenderBatch& batch,
	Taz::FrameRenderData& frameData)
{
	frameData.planeColorRenderer.initBatch(batch);

	// Fill batch data
	drawBatch(batch.entities, frameData.planeColorRenderer);
}

void AppInterface::preparePlaneModelBatch(
	Taz::GECSRenderBatch& batch,
	Taz::FrameRenderData& frameData)
{
	frameData.planeModelRenderer.initBatch(batch);

	// Fill batch data
	drawBatch(batch.entities, frameData.planeModelRenderer);
}

void AppInterface::prepareLightBatch(
	Taz::GECSRenderBatch& batch,
	Taz::FrameRenderData& frameData
)
{
	frameData.lightRenderer.initBatch(batch);

	// Fill batch data
	drawBatch(batch.entities, frameData.lightRenderer);
}


void AppInterface::renderBatch(
	const Taz::GECSRenderBatch& batch,
	Taz::FrameRenderData& frameData,
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
	Taz::FrameRenderData& frameData,
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

	if (batch.batchName == "groupPathLinks"
		) {
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(0.0f, -1.0f);
		frameData.lineRenderer.endBatch(batch);
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	else {
		frameData.lineRenderer.endBatch(batch);
	}

	shader.unuse();

}

void AppInterface::drawPlaneColorBatch(
	const Taz::GECSRenderBatch& batch,
	Taz::FrameRenderData& frameData,
	ICamera& camera)
{
	// Setup shader and render
	auto& shader = *resourceManager.getGLSLProgram(batch.shaderName);
	resourceManager.setupShader(shader, camera);

	GLint pLocation = shader.getUniformLocation("rotationMatrix");
	glUniformMatrix4fv(pLocation, 1, GL_FALSE, glm::value_ptr(batch.rotationMatrix));

	if (batch.batchName == "groupArrowHeads_0"
		) {
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(0.0f, -1.0f);
		frameData.planeColorRenderer.endBatch(batch);
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	else {
		frameData.planeColorRenderer.endBatch(batch);
	}
	shader.unuse();

}

void AppInterface::drawPlaneModelBatch(
	const Taz::GECSRenderBatch& batch,
	Taz::FrameRenderData& frameData,
	ICamera& camera)
{
	// Setup shader and render
	auto& shader = *resourceManager.getGLSLProgram(batch.shaderName);
	resourceManager.setupShader(shader, camera);

	frameData.planeModelRenderer.endBatch(batch);
	shader.unuse();

}

void AppInterface::drawLightBatch(
	const Taz::GECSRenderBatch& batch,
	Taz::FrameRenderData& frameData,
	ICamera& camera
)
{
	// Setup shader and render
	auto& shader = *resourceManager.getGLSLProgram(batch.shaderName);
	resourceManager.setupShader(shader, camera);

	frameData.lightRenderer.endBatch(batch);
	shader.unuse();

}


