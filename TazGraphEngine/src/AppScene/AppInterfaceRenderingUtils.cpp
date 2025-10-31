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


void AppInterface::renderBatch(
	const Taz::RenderBatch& batch,
	const Taz::FrameRenderData& frameData,
	ICamera& camera)
{
	auto main_camera2D = dynamic_cast<const PerspectiveCamera*>(&camera);

	switch (batch.type) {
	case Taz::RenderBatch::Type::Line:
		drawLineBatch(batch, frameData, camera);
		break;
	case Taz::RenderBatch::Type::PlaneColor:
		drawPlaneColorBatch(batch, frameData, camera);
		break;
	case Taz::RenderBatch::Type::PlaneModel:
		drawPlaneModelBatch(batch, frameData, camera);
		break;
	case Taz::RenderBatch::Type::Light:
		drawLightBatch(batch, frameData, camera);
		break;
	}
}

void AppInterface::drawLineBatch(
	const Taz::RenderBatch& batch,
	const Taz::FrameRenderData& frameData,
	ICamera& camera
)
{
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	lineRenderer.begin();
	lineRenderer.initLineBatch(batch.lineCount);
	lineRenderer.initBoxBatch(batch.boxCount);
	lineRenderer.initBatchSize();


	drawBatch(batch.entities, lineRenderer);
	// Setup shader and render
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
	const Taz::FrameRenderData& frameData,
	ICamera& camera)
{
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	planeColorRenderer.begin();
	planeColorRenderer.initQuadBatch(batch.quadCount);
	planeColorRenderer.initTriangleBatch(batch.triangleCount);
	planeColorRenderer.initBatchSize();

	// Fill batch data
	drawBatch(batch.entities, planeColorRenderer);

	// Setup shader and render
	auto& shader = *resourceManager.getGLSLProgram(batch.shaderName);
	resourceManager.setupShader(shader, camera);

	if (batch.rotationMatrix != glm::mat4(1.0f)) {
		GLint pLocation = shader.getUniformLocation("rotationMatrix");
		glUniformMatrix4fv(pLocation, 1, GL_FALSE, glm::value_ptr(batch.rotationMatrix));
	}

	planeColorRenderer.end();
	planeColorRenderer.renderBatch();
	shader.unuse();

}

void AppInterface::drawPlaneModelBatch(
	const Taz::RenderBatch& batch,
	const Taz::FrameRenderData& frameData,
	ICamera& camera)
{
	//CHange camera based on scene
	planeModelRenderer.begin();
	planeModelRenderer.initQuadBatch(batch.quadCount);
	planeModelRenderer.initBatchSize();

	// Fill batch data
	drawBatch(batch.entities, planeModelRenderer);
	// Setup shader and render
	auto& shader = *resourceManager.getGLSLProgram(batch.shaderName);
	resourceManager.setupShader(shader, camera);

	planeModelRenderer.end();
	planeModelRenderer.renderBatch();
	shader.unuse();

}

void AppInterface::drawLightBatch(
	const Taz::RenderBatch& batch,
	const Taz::FrameRenderData& frameData,
	ICamera& camera
)
{
	lightRenderer.begin();
	lightRenderer.initBoxBatch(batch.boxCount);
	lightRenderer.initSphereBatch(batch.sphereCount);
	lightRenderer.initBatchSize();

	// Fill batch data
	drawBatch(batch.entities, lightRenderer);

	// Setup shader and render
	auto& shader = *resourceManager.getGLSLProgram(batch.shaderName);
	resourceManager.setupShader(shader, camera);

	lightRenderer.end();
	lightRenderer.renderBatch();
	shader.unuse();

}


