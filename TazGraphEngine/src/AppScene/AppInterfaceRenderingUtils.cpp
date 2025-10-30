#include "./AppInterface.h"

void AppInterface::renderBatch(const Taz::RenderBatch& batch, const Taz::FrameRenderData& frameData)
{
	switch (batch.type) {
	case Taz::RenderBatch::Type::Line:
		drawLineBatch(batch, frameData);
		break;
	case Taz::RenderBatch::Type::PlaneColor:
		drawPlaneColorBatch(batch, frameData);
		break;
	case Taz::RenderBatch::Type::PlaneModel:
		drawPlaneModelBatch(batch, frameData);
		break;
	case Taz::RenderBatch::Type::Light:
		drawLightBatch(batch, frameData);
		break;
	}
}

void AppInterface::drawLineBatch(const Taz::RenderBatch& batch, const Taz::FrameRenderData& frameData)
{
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	lineRenderer.begin();
	lineRenderer.initLineBatch(batch.lineCount);
	lineRenderer.initBoxBatch(batch.boxCount);
	lineRenderer.initBatchSize();


	drawBatch(batch.entities, lineRenderer);
	// Setup shader and render
	auto& shader = *resourceManager.getGLSLProgram(batch.shaderName);
	resourceManager.setupShader(shader, *main_camera2D);

	if (batch.viewportSize != glm::vec2(0.0f)) {
		GLint pLocation = shader.getUniformLocation("viewportSize");
		glUniform2f(pLocation, batch.viewportSize.x, batch.viewportSize.y);
	}

	lineRenderer.end();
	lineRenderer.renderBatch();
	shader.unuse();

}

void AppInterface::drawPlaneColorBatch(const Taz::RenderBatch& batch, const Taz::FrameRenderData& frameData)
{
}

void AppInterface::drawPlaneModelBatch(const Taz::RenderBatch& batch, const Taz::FrameRenderData& frameData)
{
}

void AppInterface::drawLightBatch(const Taz::RenderBatch& batch, const Taz::FrameRenderData& frameData)
{
}

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

