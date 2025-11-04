#include "./AppInterface.h"


void AppInterface::drawBatch(const std::vector<EntityID>& entities, LineRenderer& batch) {

	threadPool.parallel(entities.size(), [&](int start, int end) {
		for (int i = start; i < end; i++) {
			auto* entity = _sceneList->getCurrent()->manager->getEntityFromId(entities[i]);
			entity->draw(i, batch, _window);
		}
		});

}

void AppInterface::drawBatch(const std::vector<EntityID>& entities, PlaneColorRenderer& batch) {

	threadPool.parallel(entities.size(), [&](int start, int end) {
		for (int i = start; i < end; i++) {
			auto* entity = _sceneList->getCurrent()->manager->getEntityFromId(entities[i]);

			entity->draw(i, batch, _window);
		}
		});
}

void AppInterface::drawBatch(const std::vector<EntityID>& entities, PlaneModelRenderer& batch) {
	// before calling this make sure that reserved the right amount of memory

	for (int i = 0; i < entities.size(); i++) {
		auto* entity = _sceneList->getCurrent()->manager->getEntityFromId(entities[i]);

		entity->draw(i, batch, _window);
	}

}

void AppInterface::drawBatch(const std::vector<EntityID>& entities, LightRenderer& batch) {
	// before calling this make sure that reserved the right amount of memory

	for (int i = 0; i < entities.size(); i++) {
		auto* entity = _sceneList->getCurrent()->manager->getEntityFromId(entities[i]);

		entity->draw(i, batch, _window);
	}
}

void AppInterface::prepareBatch(
	const Taz::RenderBatch& batch)
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
	const Taz::RenderBatch& batch
)
{
	lineRenderer.initLineBatch(batch.lineCount);
	lineRenderer.initBoxBatch(batch.boxCount);
	lineRenderer.initBatchSize();

	// set shader on lineRenderer.batch.back()
	// set viewport on lineRenderer.batch.back()
	// set rotationMatrix on lineRenderer.batch.back()
	drawBatch(batch.entities, lineRenderer);
}

void AppInterface::preparePlaneColorBatch(
	const Taz::RenderBatch& batch)
{
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	planeColorRenderer.initQuadBatch(batch.quadCount);
	planeColorRenderer.initTriangleBatch(batch.triangleCount);
	planeColorRenderer.initBatchSize();

	// Fill batch data
	drawBatch(batch.entities, planeColorRenderer);
}

void AppInterface::preparePlaneModelBatch(
	const Taz::RenderBatch& batch)
{
	//CHange camera based on scene
	planeModelRenderer.initQuadBatch(batch.quadCount);
	planeModelRenderer.initBatchSize();

	// Fill batch data
	drawBatch(batch.entities, planeModelRenderer);
}

void AppInterface::prepareLightBatch(
	const Taz::RenderBatch& batch
)
{
	lightRenderer.initBoxBatch(batch.boxCount);
	lightRenderer.initSphereBatch(batch.sphereCount);
	lightRenderer.initBatchSize();

	// Fill batch data
	drawBatch(batch.entities, lightRenderer);
}


void AppInterface::renderBatch(
	const Taz::RenderBatch& batch,
	ICamera& camera)
{
	auto main_camera2D = dynamic_cast<const PerspectiveCamera*>(&camera);

	switch (batch.renderer_type) {
	case Taz::RenderBatch::RendererType::Line:
		drawLineBatch(batch, camera);
		break;
	case Taz::RenderBatch::RendererType::PlaneColor:
		drawPlaneColorBatch(batch, camera);
		break;
	case Taz::RenderBatch::RendererType::PlaneModel:
		drawPlaneModelBatch(batch, camera);
		break;
	case Taz::RenderBatch::RendererType::Light:
		drawLightBatch(batch, camera);
		break;
	}
}

void AppInterface::drawLineBatch(
	const Taz::RenderBatch& batch,
	ICamera& camera
)
{
	auto& shader = *resourceManager.getGLSLProgram(batch.shaderName);
	resourceManager.setupShader(shader, camera);

	if (batch.viewportSize != glm::vec2(0.0f)) {
		GLint pLocation = shader.getUniformLocation("viewportSize");
		glUniform2f(pLocation, batch.viewportSize.x, batch.viewportSize.y);
	}

	lineRenderer.end();
	lineRenderer.renderBatch();
	shader.unuse();

}

void AppInterface::drawPlaneColorBatch(
	const Taz::RenderBatch& batch,
	ICamera& camera)
{
	// Setup shader and render
	auto& shader = *resourceManager.getGLSLProgram(batch.shaderName);
	resourceManager.setupShader(shader, camera);

	GLint pLocation = shader.getUniformLocation("rotationMatrix");
	glUniformMatrix4fv(pLocation, 1, GL_FALSE, glm::value_ptr(batch.rotationMatrix));

	planeColorRenderer.end();
	planeColorRenderer.renderBatch();
	shader.unuse();

}

void AppInterface::drawPlaneModelBatch(
	const Taz::RenderBatch& batch,
	ICamera& camera)
{
	// Setup shader and render
	auto& shader = *resourceManager.getGLSLProgram(batch.shaderName);
	resourceManager.setupShader(shader, camera);

	planeModelRenderer.end();
	planeModelRenderer.renderBatch();
	shader.unuse();

}

void AppInterface::drawLightBatch(
	const Taz::RenderBatch& batch,
	ICamera& camera
)
{
	// Setup shader and render
	auto& shader = *resourceManager.getGLSLProgram(batch.shaderName);
	resourceManager.setupShader(shader, camera);

	lightRenderer.end();
	lightRenderer.renderBatch();
	shader.unuse();

}


