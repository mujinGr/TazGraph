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


	GLSLProgram glsl_texture = *_resourceManager.getGLSLProgram("texture");
	GLSLProgram glsl_light = *_resourceManager.getGLSLProgram("light");
	GLSLProgram glsl_lineColor = *_resourceManager.getGLSLProgram("lineColor");
	GLSLProgram glsl_wireframeColor = *_resourceManager.getGLSLProgram("wireframeColor");
	GLSLProgram glsl_color = *_resourceManager.getGLSLProgram("color");
	GLSLProgram glsl_framebuffer = *_resourceManager.getGLSLProgram("framebuffer");

	glm::mat4 rotationMatrix = glm::mat4(1.0f);
	glm::vec3 cameraAimPos = main_camera2D->getAimPos();
	glm::vec3 directionToCamera = glm::normalize(cameraAimPos - main_camera2D->eyePos);
	glm::vec3 cameraEulerAngles = main_camera2D->getEulerAnglesFromDirection(directionToCamera);

	rotationMatrix = getRotationMatrix(cameraEulerAngles);

	//_PlaneModelRenderer.begin();

	/*_resourceManager.setupShader(*_resourceManager.getGLSLProgram("texture"), "worldMap", *main_camera2D);
	renderBatch(backgroundImage, _PlaneModelRenderer, false);
	_PlaneModelRenderer.end();
	_PlaneModelRenderer.renderBatch();*/
	_viewportFramebuffer.Bind();
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	////////////OPENGL USE
	glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);

	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Blending for smooth edges (premultiplied or standard)
	glEnable(GL_BLEND);
	/////////////////////////////////////////////////////
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//Grid Rendering

	_LineRenderer.begin();
	_resourceManager.setupShader(glsl_lineColor, *main_camera2D);

	_LineRenderer.initLineBatch(
		manager->getVisibleGroup<LinkEntity>(Manager::groupGridLinks).size()
	);

	_LineRenderer.initBatchSize();
	drawBatch(manager->getVisibleGroup<LinkEntity>(Manager::groupGridLinks), _LineRenderer);
	_LineRenderer.end();
	_LineRenderer.renderBatch();
	glsl_lineColor.unuse();

	// Debug Rendering
	if (renderDebug) {
		_LineRenderer.begin();
		_resourceManager.setupShader(glsl_wireframeColor, *main_camera2D);

		/*GLint viewportLoc = glsl_lineColor.getUniformLocation("_viewport");
		glUniform4f(viewportLoc, 0.0f, 0.0f, 800.0f, 640.0f);*/


		std::vector<Cell*> intercectedCells = manager->grid->getIntersectedCameraCells(*main_camera2D);

		_LineRenderer.initQuadBatch(4);

		_LineRenderer.initBoxBatch(
			intercectedCells.size() +
			manager->getVisibleGroup<NodeEntity>(Manager::groupNodes_0).size() +
			manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_0).size() +
			manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_1).size()
		);


		_LineRenderer.initBatchSize();

		size_t v_index = 0;

		_LineRenderer.drawRectangle(v_index++, glm::vec2(ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2), glm::vec3(-ROW_CELL_SIZE / 4, -COLUMN_CELL_SIZE / 4, 0.0f), TazColor(255, 0, 255, 255));
		_LineRenderer.drawRectangle(v_index++, glm::vec2(ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2), glm::vec3(ROW_CELL_SIZE / 4, -COLUMN_CELL_SIZE / 4, 0.0f), TazColor(255, 0, 255, 255));
		_LineRenderer.drawRectangle(v_index++, glm::vec2(ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2), glm::vec3(-ROW_CELL_SIZE / 4, COLUMN_CELL_SIZE / 4, 0.0f), TazColor(255, 0, 255, 255));
		_LineRenderer.drawRectangle(v_index++, glm::vec2(ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2), glm::vec3(ROW_CELL_SIZE / 4, COLUMN_CELL_SIZE / 4, 0.0f), TazColor(255, 0, 255, 255));


		size_t box_v_index = 0;

		for (const auto& cell : intercectedCells) {
			glm::vec3 cellBox_center(cell->boundingBox_center.x, cell->boundingBox_center.y, cell->boundingBox_center.z);
			glm::vec3 cellBox_size(cell->boundingBox_size.x, cell->boundingBox_size.y, cell->boundingBox_size.z);

			_LineRenderer.drawBox(box_v_index++, cellBox_size, cellBox_center, TazColor(0, 255, 0, 20));  // Drawing each cell in red for visibility
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

					_LineRenderer.drawBox(box_v_index++, nodeBox_size, nodeBox_org, TazColor(255, 255, 255, 255));  // Drawing each cell in red for visibility

					//_LineRenderer.drawCircle(glm::vec2(tr->position.x, tr->position.y), TazColor(255, 255, 255, 255), tr->getPosition().x);
					//break;
				}

			}
		}


		_LineRenderer.end();
		_LineRenderer.renderElementsBatch();
		glsl_wireframeColor.unuse();

	}

	_LineRenderer.begin();
	_PlaneColorRenderer.begin();
	_PlaneModelRenderer.begin();
	_LightRenderer.begin();
	//! Line Renderer Init
	_LineRenderer.initLineBatch(
		manager->getVisibleGroup<LinkEntity>(Manager::groupLinks_0).size() +
		manager->getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_0).size() +
		manager->getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_1).size()
	);
	//! TazColor Renderer Init
	_PlaneColorRenderer.initQuadBatch(
		manager->getVisibleGroup<NodeEntity>(Manager::groupNodes_0).size() +
		manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_0).size() +
		manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_1).size()
	);
	_PlaneColorRenderer.initTriangleBatch(
		manager->getVisibleGroup<EmptyEntity>(Manager::groupArrowHeads_0).size()
	);


	//! Model Renderer Init
	_PlaneModelRenderer.initQuadBatch(
		manager->getVisibleGroup<NodeEntity>(Manager::groupRenderSprites).size()
	);

	//! Light Renderer Init
	_LightRenderer.initBoxBatch(
		manager->getVisibleGroup<EmptyEntity>(Manager::groupEmpties).size()
	);

	_LightRenderer.initSphereBatch(
		manager->getVisibleGroup<EmptyEntity>(Manager::groupSphereEmpties).size()
	);


	_PlaneColorRenderer.initBatchSize();
	_LineRenderer.initBatchSize();
	_PlaneModelRenderer.initBatchSize();
	_LightRenderer.initBatchSize();

	std::vector<EntityID> allLinks;
	auto& links = manager->getVisibleGroup<LinkEntity>(Manager::groupLinks_0);
	auto& group0 = manager->getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_0);
	auto& group1 = manager->getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_1);

	allLinks.insert(allLinks.end(), links.begin(), links.end());
	allLinks.insert(allLinks.end(), group0.begin(), group0.end());
	allLinks.insert(allLinks.end(), group1.begin(), group1.end());

	drawBatch(allLinks, _LineRenderer);

	//_LineRenderer.renderBatch(cameraMatrix, 2.0f);

	drawBatch(manager->getVisibleGroup<NodeEntity>(Manager::groupNodes_0), _PlaneColorRenderer);
	drawBatch(manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_0), _PlaneColorRenderer);
	drawBatch(manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_1), _PlaneColorRenderer);

	drawBatch(manager->getVisibleGroup<EmptyEntity>(Manager::groupArrowHeads_0), _PlaneColorRenderer);


	drawBatch(manager->getVisibleGroup<EmptyEntity>(Manager::groupRenderSprites), _PlaneModelRenderer);

	drawBatch(manager->getVisibleGroup<NodeEntity>(Manager::groupRenderSprites), _PlaneModelRenderer);

	drawBatch(manager->getVisibleGroup<EmptyEntity>(Manager::groupEmpties), _LightRenderer);
	drawBatch(manager->getVisibleGroup<EmptyEntity>(Manager::groupSphereEmpties), _LightRenderer);




	_resourceManager.setupShader(glsl_texture, *main_camera2D);
	_PlaneModelRenderer.end();
	_PlaneModelRenderer.renderBatch(_resourceManager.getGLSLProgram("texture"));
	glsl_texture.unuse();

	_resourceManager.setupShader(glsl_color, *main_camera2D);
	GLint pLocation = glsl_color.getUniformLocation("rotationMatrix");
	glUniformMatrix4fv(pLocation, 1, GL_FALSE, glm::value_ptr(rotationMatrix));
	_PlaneColorRenderer.end();
	_PlaneColorRenderer.renderBatch(&glsl_color);
	glsl_color.unuse();


	_resourceManager.setupShader(glsl_lineColor, *main_camera2D);

	_LineRenderer.end();
	_LineRenderer.renderBatch();
	glsl_lineColor.unuse();

	_resourceManager.setupShader(glsl_light, *main_camera2D);
	_LightRenderer.end();
	_LightRenderer.renderBatch();
	glsl_light.unuse();

	//! Link Paths rendering & Ports rendering
	_LineRenderer.begin();
	_PlaneColorRenderer.begin();

	_LineRenderer.initLineBatch(
		manager->getVisibleGroup<LinkEntity>(Manager::groupPathLinks).size() +
		manager->getVisibleGroup<LinkEntity>(Manager::groupPathInnerLinks).size()
	);
	_PlaneColorRenderer.initQuadBatch(
		manager->getVisibleGroup<EmptyEntity>(Manager::groupPorts).size() +
		manager->getVisibleGroup<EmptyEntity>(Manager::groupPortSlots).size()
	);

	_LineRenderer.initBatchSize();
	_PlaneColorRenderer.initBatchSize();

	std::vector<EntityID> allPathLinks;
	auto& pathlinks = manager->getVisibleGroup<LinkEntity>(Manager::groupPathLinks);
	auto& innerLinks = manager->getVisibleGroup<LinkEntity>(Manager::groupPathInnerLinks);

	allPathLinks.insert(allPathLinks.end(), pathlinks.begin(), pathlinks.end());
	allPathLinks.insert(allPathLinks.end(), innerLinks.begin(), innerLinks.end());

	drawBatch(allPathLinks, _LineRenderer);

	std::vector<EntityID> allNodeUtils;
	auto& ports = manager->getVisibleGroup<EmptyEntity>(Manager::groupPorts);
	auto& portSlots = manager->getVisibleGroup<EmptyEntity>(Manager::groupPortSlots);

	allNodeUtils.insert(allNodeUtils.end(), ports.begin(), ports.end());
	allNodeUtils.insert(allNodeUtils.end(), portSlots.begin(), portSlots.end());

	drawBatch(allNodeUtils, _PlaneColorRenderer);

	_resourceManager.setupShader(glsl_lineColor, *main_camera2D);

	pLocation = glsl_lineColor.getUniformLocation("viewportSize");
	glUniform2f(pLocation, _window->getScreenWidth(), _window->getScreenHeight());

	_LineRenderer.end();
	_LineRenderer.renderBatch();
	glsl_lineColor.unuse();

	_resourceManager.setupShader(glsl_color, *main_camera2D);
	_PlaneColorRenderer.end();
	_PlaneColorRenderer.renderBatch(&glsl_color);
	glsl_color.unuse();



	_LineRenderer.begin();

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

	_LineRenderer.initLineBatch(
		linkCount
		//+1
	);
	_LineRenderer.initBoxBatch(
		nodeCount
	);

	_LineRenderer.initBatchSize();

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

					_LineRenderer.drawBox(boxIndex++, nodeBox_size, nodeBox_org, TazColor(255, 255, 0, 100)); //todo add angle for drawRectangle
				}
			}
			else if (link) {
				if (_selectedEntities[i].first->hasComponent<Line_w_Color>()) {
					glm::vec3 startP = link->fromPos;
					glm::vec3 endP = link->toPos;

					_LineRenderer.drawLine(lineIndex++, startP, endP, TazColor(255, 255, 0, 100), TazColor(255, 255, 0, 100), 20.0f);
				}
			}
		}
	}



	//_LineRenderer.drawLine(lineIndex++, pointAtZ0, pointAtO, TazColor(0, 0, 0, 255), TazColor(0, 0, 255, 255));

	_resourceManager.setupShader(glsl_lineColor, *main_camera2D);


	_LineRenderer.end();
	_LineRenderer.renderBatch();
	glsl_lineColor.unuse();

	_resourceManager.setupShader(glsl_wireframeColor, *main_camera2D);

	_LineRenderer.end();
	_LineRenderer.renderElementsBatch();
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


	GLSLProgram glsl_texture = *_resourceManager.getGLSLProgram("texture");
	GLSLProgram glsl_light = *_resourceManager.getGLSLProgram("light");
	GLSLProgram glsl_lineColor = *_resourceManager.getGLSLProgram("lineColor");
	GLSLProgram glsl_color = *_resourceManager.getGLSLProgram("color");
	GLSLProgram glsl_framebuffer = *_resourceManager.getGLSLProgram("framebuffer");


	_minimapFramebuffer.Bind();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/////////////////////////////////////////////////////
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);




	_PlaneColorRenderer.begin();
	_PlaneColorRenderer.initQuadBatch(
		manager->getGroup<NodeEntity>(Manager::groupMinimapNodes).size()
	);

	_PlaneColorRenderer.initBatchSize();

	drawBatch(manager->getGroup<NodeEntity>(Manager::groupMinimapNodes), _PlaneColorRenderer);

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

	_resourceManager.setupShader(glsl_color, *minimap_camera2D);

	GLint pLocation = glsl_color.getUniformLocation("rotationMatrix");
	glUniformMatrix4fv(pLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	_PlaneColorRenderer.end();
	_PlaneColorRenderer.renderBatch(&glsl_color);
	glsl_color.unuse();


	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glBindTexture(GL_TEXTURE_2D, 0);
	///////////////////////////////////////////////////////

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	_minimapFramebuffer.Unbind();
}