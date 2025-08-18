#include "Graph.h"
#include <AppScene/AppInterface.h>

void Graph::renderBatch(const std::vector<LinkEntity*>& entities, LineRenderer& batch) {
	//! activate threads near the end, where we have completed everything else
	if (manager->arrowheadsEnabled) {
		_app->threadPool.parallel(entities.size(), [&](int start, int end) {
			for (int i = start; i < end; i++) {
				assert(entities[i]->hasComponent<Line_w_Color>());

				entities[i]->GetComponent<Line_w_Color>().drawWithPorts(i, batch, *Graph::_window);
			}
			});
	}
	else {
		_app->threadPool.parallel(entities.size(), [&](int start, int end) {
			for (int i = start; i < end; i++) {
				assert(entities[i]->hasComponent<Line_w_Color>());
				entities[i]->GetComponent<Line_w_Color>().draw(i, batch, *Graph::_window);
			}
			});
	}

}

void Graph::renderBatch(const std::vector<NodeEntity*>& entities, PlaneColorRenderer& batch) {

	_app->threadPool.parallel(entities.size(), [&](int start, int end) {
		for (int i = start; i < end; i++) {
			assert(entities[i]->hasComponent<Rectangle_w_Color>());
			entities[i]->GetComponent<Rectangle_w_Color>().draw(i, batch, *Graph::_window);
		}
		});
}

void Graph::renderBatch(const std::vector<EmptyEntity*>& entities, PlaneColorRenderer& batch) {

	_app->threadPool.parallel(entities.size(), [&](int start, int end) {
		for (int i = start; i < end; i++) {
			entities[i]->draw(i, batch, *Graph::_window);
		}
		});
}

void Graph::renderBatch(const std::vector<NodeEntity*>& entities, PlaneModelRenderer& batch) {
	// before calling this make sure that reserved the right amount of memory

	for (int i = 0; i < entities.size(); i++) {
		entities[i]->draw(i, batch, *Graph::_window);
	}

}
void Graph::renderBatch(const std::vector<EmptyEntity*>& entities, PlaneModelRenderer& batch) {
	// before calling this make sure that reserved the right amount of memory

	for (int i = 0; i < entities.size(); i++) {
		entities[i]->draw(i, batch, *Graph::_window);
	}
}

void Graph::renderBatch(const std::vector<EmptyEntity*>& entities, LightRenderer& batch) {
	// before calling this make sure that reserved the right amount of memory

	for (int i = 0; i < entities.size(); i++) {
		entities[i]->draw(i, batch, *Graph::_window);
	}
}

void Graph::draw()
{
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	GLSLProgram glsl_texture = *_resourceManager.getGLSLProgram("texture");
	GLSLProgram glsl_light = *_resourceManager.getGLSLProgram("light");
	GLSLProgram glsl_lineColor = *_resourceManager.getGLSLProgram("lineColor");
	GLSLProgram glsl_color = *_resourceManager.getGLSLProgram("color");
	GLSLProgram glsl_framebuffer = *_resourceManager.getGLSLProgram("framebuffer");

	_framebuffer.Bind();

	//todo TECHNIQUE TO USE FOR MINIMAP
	//static float elapsed;
	//
	//elapsed = elapsed + getApp()->getFPSLimiter().frameTime / 1000.0f;

	//if (elapsed < 10) {
	//	_framebuffer.Unbind();

	//	glClear(GL_COLOR_BUFFER_BIT);
	//	return;
	//}
	//else {
	//	elapsed = 0;
	//}

	////////////OPENGL USE
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glDisable(GL_LINE_SMOOTH);

	/////////////////////////////////////////////////////
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


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

	// Debug Rendering
	if (_renderDebug) {
		_LineRenderer.begin();
		_resourceManager.setupShader(glsl_lineColor, *main_camera2D);

		/*GLint viewportLoc = glsl_lineColor.getUniformLocation("_viewport");
		glUniform4f(viewportLoc, 0.0f, 0.0f, 800.0f, 640.0f);*/


		std::vector<Cell*> intercectedCells = manager->grid->getIntersectedCameraCells(*main_camera2D);

		_LineRenderer.initBatchSquares(4);

		_LineRenderer.initBatchBoxes(
			intercectedCells.size() +
			manager->getVisibleGroup<NodeEntity>(Manager::groupNodes_0).size() +
			manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_0).size() +
			manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_1).size()
		);


		_LineRenderer.initBatchSize();

		size_t v_index = 0;

		_LineRenderer.drawRectangle(v_index++, glm::vec4(-ROW_CELL_SIZE / 2, -COLUMN_CELL_SIZE / 2, ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2), Color(255, 0, 255, 255), 0.0f, 0.0f);
		_LineRenderer.drawRectangle(v_index++, glm::vec4(0, -COLUMN_CELL_SIZE / 2, ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2), Color(255, 0, 255, 255), 0.0f, 0.0f);
		_LineRenderer.drawRectangle(v_index++, glm::vec4(-ROW_CELL_SIZE / 2, 0, ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2), Color(255, 0, 255, 255), 0.0f, 0.0f);
		_LineRenderer.drawRectangle(v_index++, glm::vec4(0, 0, ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2), Color(255, 0, 255, 255), 0.0f, 0.0f);


		size_t box_v_index = 0;

		for (const auto& cell : intercectedCells) {
			glm::vec3 cellBox_org(cell->boundingBox_origin.x, cell->boundingBox_origin.y, cell->boundingBox_origin.z);
			glm::vec3 cellBox_size(cell->boundingBox_size.x, cell->boundingBox_size.y, cell->boundingBox_size.z);

			_LineRenderer.drawBox(box_v_index++, cellBox_org, cellBox_size, Color(0, 255, 0, 20), 0.0f);  // Drawing each cell in red for visibility
		}

		for (auto& group : {
			manager->getVisibleGroup<NodeEntity>(Manager::groupNodes_0),
			manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_0),
			manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_1)
			}) {

			std::vector<NodeEntity*> groupVec = group;

			for (auto& entity : groupVec) {

				if (entity->hasComponent<TransformComponent>())
				{
					TransformComponent* tr = &entity->GetComponent<TransformComponent>();

					glm::vec3 nodeBox_org(tr->getPosition().x, tr->getPosition().y, tr->getPosition().z);
					glm::vec3 nodeBox_size(tr->size.x, tr->size.y, tr->size.z);

					_LineRenderer.drawBox(box_v_index++, nodeBox_org, nodeBox_size, Color(255, 255, 255, 255), 0.0f);  // Drawing each cell in red for visibility

					//_LineRenderer.drawCircle(glm::vec2(tr->position.x, tr->position.y), Color(255, 255, 255, 255), tr->getCenterTransform().x);
					//break;
				}

			}
		}


		_LineRenderer.end();
		_LineRenderer.renderBatch(main_camera2D->getScale() * 10.0f * (manager->grid->getGridLevel() + 1));
		glsl_lineColor.unuse();

	}

	_LineRenderer.begin();
	_PlaneColorRenderer.begin();
	_PlaneModelRenderer.begin();
	_LightRenderer.begin();
	//! Line Renderer Init
	_LineRenderer.initBatchLines(
		manager->getVisibleGroup<LinkEntity>(Manager::groupLinks_0).size() +
		manager->getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_0).size() +
		manager->getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_1).size()
	);
	//! Color Renderer Init
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

	std::vector<LinkEntity*> allLinks;
	auto& links = manager->getVisibleGroup<LinkEntity>(Manager::groupLinks_0);
	auto& group0 = manager->getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_0);
	auto& group1 = manager->getVisibleGroup<LinkEntity>(Manager::groupGroupLinks_1);

	allLinks.insert(allLinks.end(), links.begin(), links.end());
	allLinks.insert(allLinks.end(), group0.begin(), group0.end());
	allLinks.insert(allLinks.end(), group1.begin(), group1.end());

	renderBatch(allLinks, _LineRenderer);

	//_LineRenderer.renderBatch(cameraMatrix, 2.0f);

	renderBatch(manager->getVisibleGroup<NodeEntity>(Manager::groupNodes_0), _PlaneColorRenderer);
	renderBatch(manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_0), _PlaneColorRenderer);
	renderBatch(manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_1), _PlaneColorRenderer);

	renderBatch(manager->getVisibleGroup<EmptyEntity>(Manager::groupArrowHeads_0), _PlaneColorRenderer);


	renderBatch(manager->getVisibleGroup<EmptyEntity>(Manager::groupRenderSprites), _PlaneModelRenderer);

	renderBatch(manager->getVisibleGroup<NodeEntity>(Manager::groupRenderSprites), _PlaneModelRenderer);

	renderBatch(manager->getVisibleGroup<EmptyEntity>(Manager::groupEmpties), _LightRenderer);
	renderBatch(manager->getVisibleGroup<EmptyEntity>(Manager::groupSphereEmpties), _LightRenderer);


	_resourceManager.setupShader(glsl_color, *main_camera2D);
	GLint pLocation = glsl_color.getUniformLocation("rotationMatrix");
	glUniformMatrix4fv(pLocation, 1, GL_FALSE, glm::value_ptr(rotationMatrix));
	_PlaneColorRenderer.end();
	_PlaneColorRenderer.renderBatch(&glsl_color);
	glsl_color.unuse();


	_resourceManager.setupShader(glsl_lineColor, *main_camera2D);

	_LineRenderer.end();
	_LineRenderer.renderBatch(main_camera2D->getScale() * 5.0f);
	glsl_lineColor.unuse();

	_resourceManager.setupShader(glsl_texture, *main_camera2D);
	_PlaneModelRenderer.end();
	_PlaneModelRenderer.renderBatch(_resourceManager.getGLSLProgram("texture"));
	glsl_texture.unuse();

	_resourceManager.setupShader(glsl_light, *main_camera2D);
	_LightRenderer.end();
	_LightRenderer.renderBatch(_resourceManager.getGLSLProgram("light"));
	glsl_light.unuse();

	glDepthMask(GL_FALSE);  // don’t write to depth buffer
	glDisable(GL_DEPTH_TEST);

	//! Link Paths rendering & Ports rendering
	_LineRenderer.begin();
	_PlaneColorRenderer.begin();

	_LineRenderer.initBatchLines(
		manager->getVisibleGroup<LinkEntity>(Manager::groupPathLinks_0).size()
	);
	_PlaneColorRenderer.initQuadBatch(
		manager->getVisibleGroup<EmptyEntity>(Manager::groupPorts).size() +
		manager->getVisibleGroup<EmptyEntity>(Manager::groupPortSlots).size()
	);

	_LineRenderer.initBatchSize();
	_PlaneColorRenderer.initBatchSize();


	renderBatch(manager->getVisibleGroup<LinkEntity>(Manager::groupPathLinks_0), _LineRenderer);

	std::vector<EmptyEntity*> allNodeUtils;
	auto& ports = manager->getVisibleGroup<EmptyEntity>(Manager::groupPorts);
	auto& portSlots = manager->getVisibleGroup<EmptyEntity>(Manager::groupPortSlots);

	allNodeUtils.insert(allNodeUtils.end(), ports.begin(), ports.end());
	allNodeUtils.insert(allNodeUtils.end(), portSlots.begin(), portSlots.end());

	renderBatch(allNodeUtils, _PlaneColorRenderer);

	_resourceManager.setupShader(glsl_lineColor, *main_camera2D);

	pLocation = glsl_lineColor.getUniformLocation("lineWidth");
	glUniform1f(pLocation, 5.0f);

	_LineRenderer.end();
	_LineRenderer.renderBatch(main_camera2D->getScale() * 5.0f);
	glsl_lineColor.unuse();

	_resourceManager.setupShader(glsl_color, *main_camera2D);
	_PlaneColorRenderer.end();
	_PlaneColorRenderer.renderBatch(&glsl_color);
	glsl_color.unuse();

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

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

	_LineRenderer.initBatchLines(
		linkCount
		//+1
	);
	_LineRenderer.initBatchBoxes(
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

					glm::vec4 destRect;
					destRect.x = tr->getPosition().x;
					destRect.y = tr->getPosition().y;
					destRect.z = tr->size.x;
					destRect.w = tr->size.y;

					glm::vec3 nodeBox_org(destRect.x, destRect.y, tr->getPosition().z);
					glm::vec3 nodeBox_size(destRect.z, destRect.w, tr->size.z);

					_LineRenderer.drawBox(boxIndex++, nodeBox_org, nodeBox_size, Color(255, 255, 0, 100), 0.0f); //todo add angle for drawRectangle
				}
			}
			else if (link) {
				if (_selectedEntities[i].first->hasComponent<Line_w_Color>()) {
					Line_w_Color* lWc = &_selectedEntities[i].first->GetComponent<Line_w_Color>();

					glm::vec3 startP = lWc->entity->getFromNode()->GetComponent<TransformComponent>().getCenterTransform();
					glm::vec3 endP = lWc->entity->getToNode()->GetComponent<TransformComponent>().getCenterTransform();

					_LineRenderer.drawLine(lineIndex++, startP, endP, Color(255, 255, 0, 100), Color(255, 255, 0, 100));

				}
			}
		}
	}

	glEnable(GL_LINE_SMOOTH);//!this reduces a bit fps

	float z = 0.0f;

	for (int i = 0; i <= AXIS_CELLS; i++) {
		// Vertical lines (constant X, varying Y)
		glm::vec3 startV((i - AXIS_CELLS / 2.0f) * manager->grid->getCellSize(), -AXIS_CELLS / 2.0f * manager->grid->getCellSize(), z);
		glm::vec3 endV((i - AXIS_CELLS / 2.0f) * manager->grid->getCellSize(), AXIS_CELLS / 2.0f * manager->grid->getCellSize(), z);
		_LineRenderer.drawLine(lineIndex++, startV, endV, Color(255, 255, 255, 64), Color(255, 255, 255, 64));

		// Horizontal lines (constant Y, varying X)
		glm::vec3 startH(-AXIS_CELLS / 2.0f * manager->grid->getCellSize(), (i - AXIS_CELLS / 2.0f) * manager->grid->getCellSize(), z);
		glm::vec3 endH(AXIS_CELLS / 2.0f * manager->grid->getCellSize(), (i - AXIS_CELLS / 2.0f) * manager->grid->getCellSize(), z);
		_LineRenderer.drawLine(lineIndex++, startH, endH, Color(255, 255, 255, 64), Color(255, 255, 255, 64));
	}

	//_LineRenderer.drawLine(lineIndex++, pointAtZ0, pointAtO, Color(0, 0, 0, 255), Color(0, 0, 255, 255));

	_resourceManager.setupShader(glsl_lineColor, *main_camera2D);


	pLocation = glsl_lineColor.getUniformLocation("lineWidth");
	glUniform1f(pLocation, 5.0f);

	_LineRenderer.end();
	_LineRenderer.renderBatch(main_camera2D->getCameraRect().x / main_camera2D->getCameraRect().y);
	glsl_lineColor.unuse();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	/*drawHUD(labels, "arial");
	_resourceManager.getGLSLProgram("color")->use();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/

	glBindTexture(GL_TEXTURE_2D, 0);
	///////////////////////////////////////////////////////

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	_framebuffer.Unbind();

	glClear(GL_COLOR_BUFFER_BIT);
}


void Graph::drawHUD(const std::vector<NodeEntity*>& entities) {
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));

	_resourceManager.setupShader(*_resourceManager.getGLSLProgram("texture"), *hud_camera2D);
	renderBatch(entities, _hudPlaneModelRenderer);
}
