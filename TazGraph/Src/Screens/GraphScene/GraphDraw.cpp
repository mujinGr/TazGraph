#include "Graph.h"
#include <tracy/public/tracy/Tracy.hpp>

void Graph::drawBatch(const std::vector<EntityID>& entities, LineRenderer& batch) {

	_app->threadPool.parallel(entities.size(), [&](int start, int end) {
		for (int i = start; i < end; i++) {
			auto* entity = manager->getEntityFromId(entities[i]);
			entity->draw(i, batch, *Graph::_window);
		}
		});

}

void Graph::drawBatch(const std::vector<EntityID>& entities, PlaneColorRenderer& batch) {

	_app->threadPool.parallel(entities.size(), [&](int start, int end) {
		for (int i = start; i < end; i++) {
			auto* entity = manager->getEntityFromId(entities[i]);

			entity->draw(i, batch, *Graph::_window);
		}
		});
}

void Graph::drawBatch(const std::vector<EntityID>& entities, PlaneModelRenderer& batch) {
	// before calling this make sure that reserved the right amount of memory

	for (int i = 0; i < entities.size(); i++) {
		auto* entity = manager->getEntityFromId(entities[i]);

		entity->draw(i, batch, *Graph::_window);
	}

}

void Graph::drawBatch(const std::vector<EntityID>& entities, LightRenderer& batch) {
	// before calling this make sure that reserved the right amount of memory

	for (int i = 0; i < entities.size(); i++) {
		auto* entity = manager->getEntityFromId(entities[i]);

		entity->draw(i, batch, *Graph::_window);
	}
}

void Graph::draw()
{
	ZoneScoped;

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));
	std::shared_ptr<OrthoCamera> minimap_camera2D =
		std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("minimap"));


	GLSLProgram glsl_texture = *getApp()->resourceManager.getGLSLProgram("texture");
	GLSLProgram glsl_light = *getApp()->resourceManager.getGLSLProgram("light");
	GLSLProgram glsl_lineColor = *getApp()->resourceManager.getGLSLProgram("lineColor");
	GLSLProgram glsl_wireframeColor = *getApp()->resourceManager.getGLSLProgram("wireframeColor");
	GLSLProgram glsl_color = *getApp()->resourceManager.getGLSLProgram("color");
	GLSLProgram glsl_framebuffer = *getApp()->resourceManager.getGLSLProgram("framebuffer");

	glm::mat4 rotationMatrix = glm::mat4(1.0f);
	glm::vec3 cameraAimPos = main_camera2D->getAimPos();
	glm::vec3 directionToCamera = glm::normalize(cameraAimPos - main_camera2D->eyePos);
	glm::vec3 cameraEulerAngles = main_camera2D->getEulerAnglesFromDirection(directionToCamera);

	rotationMatrix = getRotationMatrix(cameraEulerAngles);

	//getApp()->planeModelRenderer.begin();

	/*getApp()->resourceManager.setupShader(*getApp()->resourceManager.getGLSLProgram("texture"), "worldMap", *main_camera2D);
	renderBatch(backgroundImage, getApp()->planeModelRenderer, false);
	getApp()->planeModelRenderer.end();
	getApp()->planeModelRenderer.renderBatch();*/
	_viewportFramebuffer.Bind();
	glDepthMask(GL_TRUE);
	////////////OPENGL USE
	glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);

	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Blending for smooth edges (premultiplied or standard)
	/////////////////////////////////////////////////////
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//Grid Rendering

	getApp()->lineRenderer.begin();
	getApp()->resourceManager.setupShader(glsl_lineColor, *main_camera2D);

	getApp()->lineRenderer.initLineBatch(
		manager->getVisibleGroup<LinkEntity>(Manager::groupGridLinks).size()
	);

	getApp()->lineRenderer.initBatchSize();
	drawBatch(manager->getVisibleGroup<LinkEntity>(Manager::groupGridLinks), getApp()->lineRenderer);
	getApp()->lineRenderer.end();
	getApp()->lineRenderer.renderBatch();
	glsl_lineColor.unuse();

	// Debug Rendering
	if (renderDebug) {
		getApp()->lineRenderer.begin();
		getApp()->resourceManager.setupShader(glsl_wireframeColor, *main_camera2D);

		/*GLint viewportLoc = glsl_lineColor.getUniformLocation("_viewport");
		glUniform4f(viewportLoc, 0.0f, 0.0f, 800.0f, 640.0f);*/


		std::vector<Cell*> intercectedCells = manager->grid->getIntersectedCameraCells(*main_camera2D);

		getApp()->lineRenderer.initQuadBatch(4);

		getApp()->lineRenderer.initBoxBatch(
			intercectedCells.size() +
			manager->getVisibleGroup<NodeEntity>(Manager::groupNodes_0).size() +
			manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_0).size() +
			manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_1).size()
		);


		getApp()->lineRenderer.initBatchSize();

		size_t v_index = 0;

		getApp()->lineRenderer.drawRectangle(v_index++, glm::vec2(ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2), glm::vec3(-ROW_CELL_SIZE / 4, -COLUMN_CELL_SIZE / 4, 0.0f), TazColor(255, 0, 255, 255));
		getApp()->lineRenderer.drawRectangle(v_index++, glm::vec2(ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2), glm::vec3(ROW_CELL_SIZE / 4, -COLUMN_CELL_SIZE / 4, 0.0f), TazColor(255, 0, 255, 255));
		getApp()->lineRenderer.drawRectangle(v_index++, glm::vec2(ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2), glm::vec3(-ROW_CELL_SIZE / 4, COLUMN_CELL_SIZE / 4, 0.0f), TazColor(255, 0, 255, 255));
		getApp()->lineRenderer.drawRectangle(v_index++, glm::vec2(ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2), glm::vec3(ROW_CELL_SIZE / 4, COLUMN_CELL_SIZE / 4, 0.0f), TazColor(255, 0, 255, 255));


		size_t box_v_index = 0;

		for (const auto& cell : intercectedCells) {
			glm::vec3 cellBox_center(cell->boundingBox_center.x, cell->boundingBox_center.y, cell->boundingBox_center.z);
			glm::vec3 cellBox_size(cell->boundingBox_size.x, cell->boundingBox_size.y, cell->boundingBox_size.z);

			getApp()->lineRenderer.drawBox(box_v_index++, cellBox_size, cellBox_center, TazColor(0, 255, 0, 20));  // Drawing each cell in red for visibility
		}

		for (auto& group : {
			manager->getVisibleGroup<NodeEntity>(Manager::groupNodes_0),
			manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_0),
			manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_1)
			}) {

			std::vector<EntityID> groupVec = group;

			for (auto entityId : groupVec) {
				auto* entity = manager->getEntityFromId(entityId);

				if (entity->hasComponent<TransformComponent>())
				{
					TransformComponent* tr = &entity->GetComponent<TransformComponent>();

					glm::vec3 nodeBox_org(tr->position);
					glm::vec3 nodeBox_size(tr->size.x, tr->size.y, tr->size.z);

					getApp()->lineRenderer.drawBox(box_v_index++, nodeBox_size, nodeBox_org, TazColor(255, 255, 255, 255));  // Drawing each cell in red for visibility

					//getApp()->lineRenderer.drawCircle(glm::vec2(tr->position.x, tr->position.y), TazColor(255, 255, 255, 255), tr->getPosition().x);
					//break;
				}

			}
		}


		getApp()->lineRenderer.end();
		getApp()->lineRenderer.renderElementsBatch();
		glsl_wireframeColor.unuse();

	}

	getApp()->lineRenderer.begin();
	getApp()->planeColorRenderer.begin();
	getApp()->planeModelRenderer.begin();
	getApp()->lightRenderer.begin();
	//! Line Renderer Init
	getApp()->lineRenderer.initLineBatch(
		manager->getVisibleGroup<LinkEntity>(Manager::groupLinks_0).size() +
		manager->getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_0).size() +
		manager->getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_1).size()
	);
	//! TazColor Renderer Init
	getApp()->planeColorRenderer.initQuadBatch(
		manager->getVisibleGroup<NodeEntity>(Manager::groupNodes_0).size() +
		manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_0).size() +
		manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_1).size()
	);
	getApp()->planeColorRenderer.initTriangleBatch(
		manager->getVisibleGroup<EmptyEntity>(Manager::groupArrowHeads_0).size()
	);


	//! Model Renderer Init
	getApp()->planeModelRenderer.initQuadBatch(
		manager->getVisibleGroup<NodeEntity>(Manager::groupRenderSprites).size()
	);

	//! Light Renderer Init
	getApp()->lightRenderer.initBoxBatch(
		manager->getVisibleGroup<EmptyEntity>(Manager::groupEmpties).size()
	);

	getApp()->lightRenderer.initSphereBatch(
		manager->getVisibleGroup<EmptyEntity>(Manager::groupSphereEmpties).size()
	);


	getApp()->planeColorRenderer.initBatchSize();
	getApp()->lineRenderer.initBatchSize();
	getApp()->planeModelRenderer.initBatchSize();
	getApp()->lightRenderer.initBatchSize();

	std::vector<EntityID> allLinks;
	auto& links = manager->getVisibleGroup<LinkEntity>(Manager::groupLinks_0);
	auto& group0 = manager->getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_0);
	auto& group1 = manager->getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_1);

	allLinks.insert(allLinks.end(), links.begin(), links.end());
	allLinks.insert(allLinks.end(), group0.begin(), group0.end());
	allLinks.insert(allLinks.end(), group1.begin(), group1.end());

	drawBatch(allLinks, getApp()->lineRenderer);

	//getApp()->lineRenderer.renderBatch(cameraMatrix, 2.0f);

	drawBatch(manager->getVisibleGroup<NodeEntity>(Manager::groupNodes_0), getApp()->planeColorRenderer);
	drawBatch(manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_0), getApp()->planeColorRenderer);
	drawBatch(manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_1), getApp()->planeColorRenderer);

	drawBatch(manager->getVisibleGroup<EmptyEntity>(Manager::groupArrowHeads_0), getApp()->planeColorRenderer);


	drawBatch(manager->getVisibleGroup<EmptyEntity>(Manager::groupRenderSprites), getApp()->planeModelRenderer);

	drawBatch(manager->getVisibleGroup<NodeEntity>(Manager::groupRenderSprites), getApp()->planeModelRenderer);

	drawBatch(manager->getVisibleGroup<EmptyEntity>(Manager::groupEmpties), getApp()->lightRenderer);
	drawBatch(manager->getVisibleGroup<EmptyEntity>(Manager::groupSphereEmpties), getApp()->lightRenderer);




	getApp()->resourceManager.setupShader(glsl_texture, *main_camera2D);
	getApp()->planeModelRenderer.end();
	getApp()->planeModelRenderer.renderBatch(getApp()->resourceManager.getGLSLProgram("texture"));
	glsl_texture.unuse();

	getApp()->resourceManager.setupShader(glsl_color, *main_camera2D);
	GLint pLocation = glsl_color.getUniformLocation("rotationMatrix");
	glUniformMatrix4fv(pLocation, 1, GL_FALSE, glm::value_ptr(rotationMatrix));
	getApp()->planeColorRenderer.end();
	getApp()->planeColorRenderer.renderBatch(&glsl_color);
	glsl_color.unuse();


	getApp()->resourceManager.setupShader(glsl_lineColor, *main_camera2D);

	getApp()->lineRenderer.end();
	getApp()->lineRenderer.renderBatch();
	glsl_lineColor.unuse();

	getApp()->resourceManager.setupShader(glsl_light, *main_camera2D);
	getApp()->lightRenderer.end();
	getApp()->lightRenderer.renderBatch();
	glsl_light.unuse();

	//! Link Paths rendering & Ports rendering
	getApp()->lineRenderer.begin();
	getApp()->planeColorRenderer.begin();

	getApp()->lineRenderer.initLineBatch(
		manager->getVisibleGroup<LinkEntity>(Manager::groupPathLinks).size() +
		manager->getVisibleGroup<LinkEntity>(Manager::groupPathInnerLinks).size()
	);
	getApp()->planeColorRenderer.initQuadBatch(
		manager->getVisibleGroup<EmptyEntity>(Manager::groupPorts).size() +
		manager->getVisibleGroup<EmptyEntity>(Manager::groupPortSlots).size()
	);

	getApp()->lineRenderer.initBatchSize();
	getApp()->planeColorRenderer.initBatchSize();

	std::vector<EntityID> allPathLinks;
	auto& pathlinks = manager->getVisibleGroup<LinkEntity>(Manager::groupPathLinks);
	auto& innerLinks = manager->getVisibleGroup<LinkEntity>(Manager::groupPathInnerLinks);

	allPathLinks.insert(allPathLinks.end(), pathlinks.begin(), pathlinks.end());
	allPathLinks.insert(allPathLinks.end(), innerLinks.begin(), innerLinks.end());

	drawBatch(allPathLinks, getApp()->lineRenderer);

	std::vector<EntityID> allNodeUtils;
	auto& ports = manager->getVisibleGroup<EmptyEntity>(Manager::groupPorts);
	auto& portSlots = manager->getVisibleGroup<EmptyEntity>(Manager::groupPortSlots);

	allNodeUtils.insert(allNodeUtils.end(), ports.begin(), ports.end());
	allNodeUtils.insert(allNodeUtils.end(), portSlots.begin(), portSlots.end());

	drawBatch(allNodeUtils, getApp()->planeColorRenderer);

	getApp()->resourceManager.setupShader(glsl_lineColor, *main_camera2D);

	pLocation = glsl_lineColor.getUniformLocation("viewportSize");
	glUniform2f(pLocation, _window->getScreenWidth(), _window->getScreenHeight());

	getApp()->lineRenderer.end();
	getApp()->lineRenderer.renderBatch();
	glsl_lineColor.unuse();

	getApp()->resourceManager.setupShader(glsl_color, *main_camera2D);
	getApp()->planeColorRenderer.end();
	getApp()->planeColorRenderer.renderBatch(&glsl_color);
	glsl_color.unuse();



	getApp()->lineRenderer.begin();

	size_t nodeCount = std::count_if(_selectedEntities.begin(), _selectedEntities.end(),
		[](const std::pair<Entity*, glm::vec3>& entry) {
			return (dynamic_cast<NodeEntity*>(entry.first) != nullptr)
				|| (dynamic_cast<EmptyEntity*>(entry.first) != nullptr);
		});

	size_t linkCount = std::count_if(_selectedEntities.begin(), _selectedEntities.end(),
		[](const std::pair<Entity*, glm::vec3>& entry) {
			return dynamic_cast<LinkEntity*>(entry.first) != nullptr;
		});

	linkCount += 2 * AXIS_CELLS + 2;

	getApp()->lineRenderer.initLineBatch(
		linkCount
		//+1
	);
	getApp()->lineRenderer.initBoxBatch(
		nodeCount
	);

	getApp()->lineRenderer.initBatchSize();

	size_t lineIndex = 0;
	size_t boxIndex = 0;


	if (!_selectedEntities.empty()) {

		for (int i = 0; i < _selectedEntities.size(); i++) {
			Node* node = dynamic_cast<Node*>(_selectedEntities[i].first);
			Empty* empty = dynamic_cast<Empty*>(_selectedEntities[i].first);
			Link* link = dynamic_cast<Link*>(_selectedEntities[i].first);
			if (node || empty) {
				if (_selectedEntities[i].first->hasComponent<TransformComponent>()) {
					TransformComponent* tr = &_selectedEntities[i].first->GetComponent<TransformComponent>();

					glm::vec3 nodePos = tr->getPosition();

					glm::vec3 nodeBox_org(nodePos.x, nodePos.y, nodePos.z);
					glm::vec3 nodeBox_size(tr->size.x, tr->size.y, tr->size.z);

					getApp()->lineRenderer.drawBox(boxIndex++, nodeBox_size, nodeBox_org, TazColor(255, 255, 0, 100)); //todo add angle for drawRectangle
				}
			}
			else if (link) {
				if (_selectedEntities[i].first->hasComponent<Line_w_Color>()) {
					glm::vec3 startP = link->fromPos;
					glm::vec3 endP = link->toPos;

					getApp()->lineRenderer.drawLine(lineIndex++, startP, endP, TazColor(255, 255, 0, 100), TazColor(255, 255, 0, 100), 20.0f);
				}
			}
		}
	}



	//getApp()->lineRenderer.drawLine(lineIndex++, pointAtZ0, pointAtO, TazColor(0, 0, 0, 255), TazColor(0, 0, 255, 255));

	getApp()->resourceManager.setupShader(glsl_lineColor, *main_camera2D);


	getApp()->lineRenderer.end();
	getApp()->lineRenderer.renderBatch();
	glsl_lineColor.unuse();

	getApp()->resourceManager.setupShader(glsl_wireframeColor, *main_camera2D);

	getApp()->lineRenderer.end();
	getApp()->lineRenderer.renderElementsBatch();
	glsl_wireframeColor.unuse();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glBindTexture(GL_TEXTURE_2D, 0);
	///////////////////////////////////////////////////////

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	_viewportFramebuffer.Unbind();

	minimapDraw();


}

void Graph::minimapDraw() {

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));
	std::shared_ptr<OrthoCamera> minimap_camera2D =
		std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("minimap"));


	GLSLProgram glsl_texture = *getApp()->resourceManager.getGLSLProgram("texture");
	GLSLProgram glsl_light = *getApp()->resourceManager.getGLSLProgram("light");
	GLSLProgram glsl_lineColor = *getApp()->resourceManager.getGLSLProgram("lineColor");
	GLSLProgram glsl_color = *getApp()->resourceManager.getGLSLProgram("color");
	GLSLProgram glsl_framebuffer = *getApp()->resourceManager.getGLSLProgram("framebuffer");


	_minimapFramebuffer.Bind();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/////////////////////////////////////////////////////
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	getApp()->planeColorRenderer.begin();
	getApp()->planeColorRenderer.initQuadBatch(
		manager->getGroup<NodeEntity>(Manager::groupMinimapNodes).size()
	);

	getApp()->planeColorRenderer.initBatchSize();

	drawBatch(manager->getGroup<NodeEntity>(Manager::groupMinimapNodes), getApp()->planeColorRenderer);

	float maxDistance = manager->grid->getNumXCells() * manager->grid->getCellSize();

	minimap_camera2D->setPosition_X(0.0f);
	minimap_camera2D->setPosition_Y(0.0f);

	glm::mat4 newProjection = glm::ortho(-maxDistance / 2.0f, maxDistance / 2.0f, -maxDistance / 2.0f, maxDistance / 2.0f);
	minimap_camera2D->setProjMatrix(newProjection);
	minimap_camera2D->setAimPos(glm::vec3(0.0f));

	float halfSize = 1000.0f;  // shows +/- 100 units around center
	float near = 0.1f;
	float far = 2000.0f;

	glm::mat4 proj = glm::ortho(-maxDistance / 2.0f, maxDistance / 2.0f, -maxDistance / 2.0f, maxDistance / 2.0f, near, far);
	minimap_camera2D->setProjMatrix(proj);

	getApp()->resourceManager.setupShader(glsl_color, *minimap_camera2D);

	GLint pLocation = glsl_color.getUniformLocation("rotationMatrix");
	glUniformMatrix4fv(pLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	getApp()->planeColorRenderer.end();
	getApp()->planeColorRenderer.renderBatch(&glsl_color);
	glsl_color.unuse();


	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glBindTexture(GL_TEXTURE_2D, 0);
	///////////////////////////////////////////////////////

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	_minimapFramebuffer.Unbind();
}