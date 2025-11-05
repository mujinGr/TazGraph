#include "Graph.h"
#include <tracy/public/tracy/Tracy.hpp>

void Graph::prepareDraw()
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
	frameData.batches.clear();

	// 0. Grid Rendering
	if (showGrid) {
		//! Prepare Frame
		Taz::GECSRenderBatch gridBatch;
		gridBatch.renderer_type = Taz::RenderBatch::RendererType::Line;
		gridBatch.mesh_type = Taz::RenderBatch::MeshType::Line;
		gridBatch.shaderName = "lineColor";
		gridBatch.entities = manager->collectEntities({
			Manager::groupGridLinks,
			}, Taz::EntityType::Link);
		gridBatch.count = gridBatch.entities.size();
		frameData.batches.push_back(gridBatch);
	}
	// 1. Nodes Batch (Color)
	{
		Taz::GECSRenderBatch nodesBatch;
		nodesBatch.renderer_type = Taz::RenderBatch::RendererType::PlaneColor;
		nodesBatch.mesh_type = Taz::RenderBatch::MeshType::Quad;

		nodesBatch.shaderName = "color";
		nodesBatch.entities = manager->collectEntities({
			Manager::groupNodes_0,
			Manager::groupGroupNodes_0,
			Manager::groupGroupNodes_1
			}, Taz::EntityType::Node);
		nodesBatch.count = nodesBatch.entities.size();
		nodesBatch.rotationMatrix = rotationMatrix;
		frameData.batches.push_back(nodesBatch);
	}
	// 2. Link Rendering
	{
		Taz::GECSRenderBatch linksBatch;
		linksBatch.renderer_type = Taz::RenderBatch::RendererType::Line;
		linksBatch.mesh_type = Taz::RenderBatch::MeshType::Line;

		linksBatch.shaderName = "lineColor";
		linksBatch.entities = manager->collectEntities({
			Manager::groupLinks_0,
			Manager::groupGroupLinks_0,
			Manager::groupGroupLinks_1
			}, Taz::EntityType::Link);
		linksBatch.count = linksBatch.entities.size();
		frameData.batches.push_back(linksBatch);

	}
	// 3. ArrowHeads Batch
	{
		Taz::GECSRenderBatch arrowsBatch;
		arrowsBatch.renderer_type = Taz::RenderBatch::RendererType::PlaneColor;
		arrowsBatch.mesh_type = Taz::RenderBatch::MeshType::Triangle;

		arrowsBatch.shaderName = "color";
		arrowsBatch.entities = manager->collectEntities({
			Manager::groupArrowHeads_0,
			}, Taz::EntityType::Empty);

		arrowsBatch.count = arrowsBatch.entities.size();
		arrowsBatch.rotationMatrix = rotationMatrix;
		frameData.batches.push_back(arrowsBatch);
	}
	// 4. Sprite Models Batch
	{
		Taz::GECSRenderBatch spritesBatch;
		spritesBatch.renderer_type = Taz::RenderBatch::RendererType::PlaneModel;
		spritesBatch.mesh_type = Taz::RenderBatch::MeshType::Quad;

		spritesBatch.shaderName = "texture";
		spritesBatch.entities = manager->collectEntities({
			Manager::groupRenderSprites
			}, Taz::EntityType::Empty);
		spritesBatch.count = spritesBatch.entities.size();
		frameData.batches.push_back(spritesBatch);
	}
	// 5.1. Lighted Boxes Batch
	{
		Taz::GECSRenderBatch lightsBatch;
		lightsBatch.renderer_type = Taz::RenderBatch::RendererType::Light;
		lightsBatch.mesh_type = Taz::RenderBatch::MeshType::Quad;

		lightsBatch.shaderName = "light";

		lightsBatch.entities = manager->collectEntities({
			Manager::groupEmpties
			}, Taz::EntityType::Empty);

		lightsBatch.count = lightsBatch.entities.size();

		frameData.batches.push_back(lightsBatch);
	}
	// 5.2. Lighted Spheres Batch
	{
		Taz::GECSRenderBatch lightsBatch;
		lightsBatch.renderer_type = Taz::RenderBatch::RendererType::Light;
		lightsBatch.mesh_type = Taz::RenderBatch::MeshType::Sphere;

		lightsBatch.shaderName = "light";

		lightsBatch.entities = manager->collectEntities({
			Manager::groupSphereEmpties
			}, Taz::EntityType::Empty);

		lightsBatch.count = lightsBatch.entities.size();

		frameData.batches.push_back(lightsBatch);
	}
	// 6. Path Links Batch (rendered without depth test)
	{
		Taz::GECSRenderBatch pathLinksBatch;
		pathLinksBatch.renderer_type = Taz::RenderBatch::RendererType::Line;
		pathLinksBatch.mesh_type = Taz::RenderBatch::MeshType::Line;

		pathLinksBatch.shaderName = "lineColor";
		pathLinksBatch.entities = manager->collectEntities({
			Manager::groupPathLinks,
			Manager::groupPathInnerLinks
			}, Taz::EntityType::Link);
		pathLinksBatch.count = pathLinksBatch.entities.size();
		pathLinksBatch.viewportSize =
			glm::vec2(
				getApp()->_window.getScreenWidth(),
				getApp()->_window.getScreenHeight()
			);
		frameData.batches.push_back(pathLinksBatch);
	}
	// 7. Ports and Slots Batch
	{
		Taz::GECSRenderBatch portsBatch;
		portsBatch.renderer_type = Taz::RenderBatch::RendererType::PlaneColor;
		portsBatch.mesh_type = Taz::RenderBatch::MeshType::Quad;

		portsBatch.shaderName = "color";
		portsBatch.entities = manager->collectEntities({
			Manager::groupPorts,
			Manager::groupPortSlots
			}, Taz::EntityType::Empty);
		portsBatch.count = portsBatch.entities.size();
		frameData.batches.push_back(portsBatch);
	}
	manager->removeAllEntitiesFromEmptyGroup(Manager::groupSelectedEntities);
	// 8.1. Selection Nodes Overlay Batch
	{
		Taz::GECSRenderBatch selectionBatch;
		selectionBatch.renderer_type = Taz::RenderBatch::RendererType::Line;
		selectionBatch.mesh_type = Taz::RenderBatch::MeshType::Box;

		selectionBatch.shaderName = "lineColor";

		size_t nodeCount = std::count_if(_selectedEntities.begin(), _selectedEntities.end(),
			[](const auto& entry) {
				return dynamic_cast<NodeEntity*>(entry.first) || dynamic_cast<EmptyEntity*>(entry.first);
			});

		selectionBatch.count = nodeCount;


		for (auto& [entity, offset] : _selectedEntities) {
			Node* node = dynamic_cast<Node*>(entity);
			Empty* empty = dynamic_cast<Empty*>(entity);

			if (node || empty) {
				auto& selectedEntity = manager->addEntity<Empty>();
				selectedEntity.addGroup(Manager::groupSelectedEntities);
				selectedEntity.addComponent<BoxComponent>();
				manager->grid->addEmpty(&selectedEntity, manager->grid->getGridLevel());

				auto& tr = entity->GetComponent<TransformComponent>();
				glm::vec3 nodeBox_org(tr.position.x, tr.position.y, tr.position.z);
				glm::vec3 nodeBox_size(tr.size.x, tr.size.y, tr.size.z);

				selectionBatch.count++;
				selectionBatch.entities.push_back(selectedEntity.getId());
			}
		}
		frameData.batches.push_back(selectionBatch);
	}
	// 8.2. Selection Links Overlay Batch
	{
		Taz::GECSRenderBatch selectionBatch;
		selectionBatch.renderer_type = Taz::RenderBatch::RendererType::Line;
		selectionBatch.mesh_type = Taz::RenderBatch::MeshType::Box;

		selectionBatch.shaderName = "lineColor";

		size_t linkCount = std::count_if(_selectedEntities.begin(), _selectedEntities.end(),
			[](const auto& entry) {
				return dynamic_cast<LinkEntity*>(entry.first);
			});

		selectionBatch.count = linkCount;

		for (auto& [entity, offset] : _selectedEntities) {
			Link* link = dynamic_cast<Link*>(entity);

			if (link) {
				auto& selectedEntity = manager->addEntity<Link>();

				selectedEntity.addComponent<Line_w_Color>();
				manager->grid->addLink(&selectedEntity, manager->grid->getGridLevel());

				selectionBatch.count++;
				selectionBatch.entities.push_back(selectedEntity.getId());
			}

		}
		frameData.batches.push_back(selectionBatch);
	}

	// Debug Rendering
	//if (renderDebug) {
	//	getApp()->lineRenderer.begin();
	//	getApp()->resourceManager.setupShader(glsl_wireframeColor, *main_camera2D);

	//	/*GLint viewportLoc = glsl_lineColor.getUniformLocation("_viewport");
	//	glUniform4f(viewportLoc, 0.0f, 0.0f, 800.0f, 640.0f);*/


	//	std::vector<Cell*> intercectedCells = manager->grid->getIntersectedCameraCells(*main_camera2D);

	//	getApp()->lineRenderer.initBatchSize2(4);

	//	getApp()->lineRenderer.initBatchSize2(
	//		intercectedCells.size() +
	//		manager->getVisibleGroup<NodeEntity>(Manager::groupNodes_0).size() +
	//		manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_0).size() +
	//		manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_1).size()
	//	);


	//	getApp()->lineRenderer.initBatchSize();

	//	size_t v_index = 0;

	//	getApp()->lineRenderer.drawRectangle(v_index++, glm::vec2(ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2), glm::vec3(-ROW_CELL_SIZE / 4, -COLUMN_CELL_SIZE / 4, 0.0f), TazColor(255, 0, 255, 255));
	//	getApp()->lineRenderer.drawRectangle(v_index++, glm::vec2(ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2), glm::vec3(ROW_CELL_SIZE / 4, -COLUMN_CELL_SIZE / 4, 0.0f), TazColor(255, 0, 255, 255));
	//	getApp()->lineRenderer.drawRectangle(v_index++, glm::vec2(ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2), glm::vec3(-ROW_CELL_SIZE / 4, COLUMN_CELL_SIZE / 4, 0.0f), TazColor(255, 0, 255, 255));
	//	getApp()->lineRenderer.drawRectangle(v_index++, glm::vec2(ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2), glm::vec3(ROW_CELL_SIZE / 4, COLUMN_CELL_SIZE / 4, 0.0f), TazColor(255, 0, 255, 255));


	//	size_t box_v_index = 0;

	//	for (const auto& cell : intercectedCells) {
	//		glm::vec3 cellBox_center(cell->boundingBox_center.x, cell->boundingBox_center.y, cell->boundingBox_center.z);
	//		glm::vec3 cellBox_size(cell->boundingBox_size.x, cell->boundingBox_size.y, cell->boundingBox_size.z);

	//		getApp()->lineRenderer.drawBox(box_v_index++, cellBox_size, cellBox_center, TazColor(0, 255, 0, 20));  // Drawing each cell in red for visibility
	//	}

	//	for (auto& group : {
	//		manager->getVisibleGroup<NodeEntity>(Manager::groupNodes_0),
	//		manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_0),
	//		manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_1)
	//		}) {

	//		std::vector<EntityID> groupVec = group;

	//		for (auto entityId : groupVec) {
	//			auto* entity = manager->getEntityFromId(entityId);

	//			if (entity->hasComponent<TransformComponent>())
	//			{
	//				TransformComponent* tr = &entity->GetComponent<TransformComponent>();

	//				glm::vec3 nodeBox_org(tr->position);
	//				glm::vec3 nodeBox_size(tr->size.x, tr->size.y, tr->size.z);

	//				getApp()->lineRenderer.drawBox(box_v_index++, nodeBox_size, nodeBox_org, TazColor(255, 255, 255, 255));  // Drawing each cell in red for visibility

	//				//getApp()->lineRenderer.drawCircle(glm::vec2(tr->position.x, tr->position.y), TazColor(255, 255, 255, 255), tr->getPosition().x);
	//				//break;
	//			}

	//		}
	//	}


	//	getApp()->lineRenderer.end();
	//	getApp()->lineRenderer.renderElementsBatch();
	//	glsl_wireframeColor.unuse();

	//}

	getApp()->planeColorRenderer.begin();
	getApp()->lineRenderer.begin();
	getApp()->planeModelRenderer.begin();
	getApp()->lightRenderer.begin();
	//minimapPrepareDraw();

	getApp()->planeColorRenderer.begin();
	getApp()->lineRenderer.begin();
	getApp()->planeModelRenderer.begin();
	getApp()->lightRenderer.begin();

	//! Prepare Draw Batches by Frame
	{
		for (const auto& batch : frameData.batches) {
			getApp()->prepareBatch(batch);
		}
	}


}

void Graph::renderDraw()
{
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


	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	{
		//! render Frame
		for (const auto& batch : frameData.batches) {
			getApp()->renderBatch(batch, *main_camera2D);
		}
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glBindTexture(GL_TEXTURE_2D, 0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	_viewportFramebuffer.Unbind();

	minimapRenderDraw();
}


void Graph::minimapPrepareDraw() {

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));
	std::shared_ptr<OrthoCamera> minimap_camera2D =
		std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("minimap"));

	GLSLProgram glsl_texture = *getApp()->resourceManager.getGLSLProgram("texture");
	GLSLProgram glsl_light = *getApp()->resourceManager.getGLSLProgram("light");
	GLSLProgram glsl_lineColor = *getApp()->resourceManager.getGLSLProgram("lineColor");
	GLSLProgram glsl_color = *getApp()->resourceManager.getGLSLProgram("color");
	GLSLProgram glsl_framebuffer = *getApp()->resourceManager.getGLSLProgram("framebuffer");


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

	glm::mat4 minimap_rotationMatrix = glm::mat4(1.0f);

	{
		Taz::GECSRenderBatch minimapBatch;
		minimapBatch.renderer_type = Taz::RenderBatch::RendererType::PlaneColor;
		minimapBatch.shaderName = "color";
		minimapBatch.entities = manager->collectEntities({
			Manager::groupMinimapNodes,
			}, Taz::EntityType::Minimap);
		minimapBatch.count = minimapBatch.entities.size();

		minimapBatch.rotationMatrix = minimap_rotationMatrix;
		frameData.batches.push_back(minimapBatch);
	}
	//! Prepare Draw Batches by Frame
	{
		for (const auto& batch : frameData.batches) {
			getApp()->prepareBatch(batch);
		}
	}
}

void Graph::minimapRenderDraw() {

	std::shared_ptr<OrthoCamera> minimap_camera2D =
		std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("minimap"));


	_minimapFramebuffer.Bind();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/////////////////////////////////////////////////////
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	{
		//! render Frame
		for (const auto& batch : frameData.batches) {
			//getApp()->renderBatch(batch, *minimap_camera2D);
		}
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glBindTexture(GL_TEXTURE_2D, 0);
	///////////////////////////////////////////////////////

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	_minimapFramebuffer.Unbind();
}