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
	int writeIndex = 1 - activeFrameIndex.load();

	auto& frameData = frameDataBuffers[writeIndex];

	rotationMatrix = getRotationMatrix(cameraEulerAngles);
	frameData.batches.clear();

	getApp()->planeColorRenderer.begin();
	getApp()->lineRenderer.begin();
	getApp()->planeModelRenderer.begin();
	getApp()->lightRenderer.begin();

	// 0. Grid Rendering
	if (showGrid) {
		//! Prepare Frame
		Taz::GECSRenderBatch gridBatch;
		gridBatch.renderer_type = Taz::RenderBatch::RendererType::Line;
		gridBatch.mesh_type = Taz::RenderBatch::MeshType::Line;

		gridBatch.batchName = manager->getGroupName(Manager::groupGridLinks);
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

		nodesBatch.batchName = manager->getGroupName(Manager::groupNodes_0);
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

		linksBatch.batchName = manager->getGroupName(Manager::groupLinks_0);
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

		arrowsBatch.batchName = manager->getGroupName(Manager::groupArrowHeads_0);
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

		spritesBatch.batchName = manager->getGroupName(Manager::groupRenderSprites);
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


		lightsBatch.batchName = manager->getGroupName(Manager::groupEmpties);
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


		lightsBatch.batchName = manager->getGroupName(Manager::groupSphereEmpties);
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

		pathLinksBatch.batchName = manager->getGroupName(Manager::groupPathLinks);
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

		portsBatch.batchName = manager->getGroupName(Manager::groupPorts);
		portsBatch.shaderName = "color";
		portsBatch.entities = manager->collectEntities({
			Manager::groupPorts,
			Manager::groupPortSlots
			}, Taz::EntityType::Empty);
		portsBatch.count = portsBatch.entities.size();
		frameData.batches.push_back(portsBatch);
	}
	// 8.1. Selection Nodes Overlay Batch
	{
		Taz::GECSRenderBatch selectionBatch;
		selectionBatch.renderer_type = Taz::RenderBatch::RendererType::Line;
		selectionBatch.mesh_type = Taz::RenderBatch::MeshType::Box;


		selectionBatch.batchName = manager->getGroupName(Manager::groupSelectedEntities);
		selectionBatch.shaderName = "wireframeColor";

		size_t nodeCount = std::count_if(_selectedEntities.begin(), _selectedEntities.end(),
			[manager = this->manager](const auto& entry) {
				Entity* ent = manager->getEntityFromId(entry.realEntityId);
				return dynamic_cast<NodeEntity*>(ent)
					|| dynamic_cast<EmptyEntity*>(ent);
			});

		selectionBatch.count = nodeCount;


		for (auto& sel : _selectedEntities) {
			Entity* realEnt = manager->getEntityFromId(sel.realEntityId);
			if (!realEnt) continue;

			Node* node = dynamic_cast<Node*>(realEnt);
			Empty* empty = dynamic_cast<Empty*>(realEnt);

			if (node || empty) {

				// Reuse overlay entity, do NOT create new one
				Empty* overlayEnt = dynamic_cast<Empty*>(manager->getEntityFromId(sel.overlayEntityId));

				if (!overlayEnt || std::get<int>(sel.overlayEntityId) < 0) {
					// Overlay somehow missing, recreate it
					auto& newEnt = manager->addEntity<Empty>();
					newEnt.addGroup(Manager::groupSelectedEntities);
					newEnt.addComponent<BoxComponent>();
					newEnt.GetComponent<BoxComponent>().color = TazColor(255, 255, 0, 255);
					manager->grid->addEmpty(&newEnt, manager->grid->getGridLevel());
					sel.overlayEntityId = newEnt.getId();
					overlayEnt = &newEnt;
				}

				// Update overlay transform here to match real entity
				overlayEnt->GetComponent<TransformComponent>().position =
					realEnt->GetComponent<TransformComponent>().getPosition();
				overlayEnt->GetComponent<TransformComponent>().size =
					realEnt->GetComponent<TransformComponent>().size;

				selectionBatch.entities.push_back(sel.overlayEntityId);
			}
		}
		frameData.batches.push_back(selectionBatch);
	}
	// 8.2. Selection Links Overlay Batch
	{
		Taz::GECSRenderBatch selectionBatch;
		selectionBatch.renderer_type = Taz::RenderBatch::RendererType::Line;
		selectionBatch.mesh_type = Taz::RenderBatch::MeshType::Line;


		selectionBatch.batchName = manager->getGroupName(Manager::groupSelectedEntities);
		selectionBatch.shaderName = "lineColor";

		size_t linkCount = std::count_if(_selectedEntities.begin(), _selectedEntities.end(),
			[manager = this->manager](const auto& entry) {
				Entity* ent = manager->getEntityFromId(entry.realEntityId);
				return dynamic_cast<LinkEntity*>(ent);
			});

		selectionBatch.count = linkCount;

		for (auto& sel : _selectedEntities) {
			LinkEntity* realEnt = dynamic_cast<LinkEntity*>(manager->getEntityFromId(sel.realEntityId));
			if (!realEnt) continue;

			Link* link = dynamic_cast<Link*>(realEnt);

			if (link) {
				Link* overlayEnt = dynamic_cast<Link*>(manager->getEntityFromId(sel.overlayEntityId));

				if (!overlayEnt || std::get<int>(sel.overlayEntityId) < 0) {
					// Overlay somehow missing, recreate it
					auto& newEnt = manager->addEntity<Link>();
					newEnt.addGroup(Manager::groupSelectedEntities);
					newEnt.addComponent<Line_w_Color>();

					newEnt.GetComponent<Line_w_Color>().setDestColor(TazColor(255, 255, 0, 255));
					newEnt.GetComponent<Line_w_Color>().setSrcColor(TazColor(255, 255, 0, 255));
					newEnt.GetComponent<Line_w_Color>().width = 10;

					manager->grid->addLink(&newEnt, manager->grid->getGridLevel());

					sel.overlayEntityId = newEnt.getId();
					overlayEnt = &newEnt;
				}

				overlayEnt->fromId =
					realEnt->fromId;
				overlayEnt->toId =
					realEnt->toId;

				overlayEnt->fromPort =
					realEnt->fromPort;
				overlayEnt->toPort =
					realEnt->toPort;

				overlayEnt->type = realEnt->type;
				overlayEnt->updateConnectionPositions();

				selectionBatch.entities.push_back(sel.overlayEntityId);
			}
		}

		frameData.batches.push_back(selectionBatch);
	}

	if (last_renderDebug && !renderDebug) {
		last_renderDebug = renderDebug;

		manager->removeAllEntitiesFromEmptyGroup(Manager::groupDebugBoxEntities);
		manager->removeAllEntitiesFromEmptyGroup(Manager::groupDebugRectangleEntities);
	}

	if (!last_renderDebug && renderDebug) {
		last_renderDebug = renderDebug;
		auto makeQuad = [&](glm::vec3 size, glm::vec3 pos) {
			auto& e = manager->addEntity<Empty>();
			e.addGroup(Manager::groupDebugRectangleEntities);
			e.addComponent<Rectangle_w_Color>();
			auto& c = e.addComponent<TransformComponent>();

			c.size = size;
			c.position = pos;
			return e.getId();
			};

		makeQuad({ ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2,0 }, { -ROW_CELL_SIZE / 4, -COLUMN_CELL_SIZE / 4, 0 });
		makeQuad({ ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2,0 }, { ROW_CELL_SIZE / 4, -COLUMN_CELL_SIZE / 4, 0 });
		makeQuad({ ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2,0 }, { -ROW_CELL_SIZE / 4,  COLUMN_CELL_SIZE / 4, 0 });
		makeQuad({ ROW_CELL_SIZE / 2, COLUMN_CELL_SIZE / 2,0 }, { ROW_CELL_SIZE / 4,  COLUMN_CELL_SIZE / 4, 0 });

		for (auto cell : manager->grid->getIntersectedCameraCells(*main_camera2D))
		{
			auto& e = manager->addEntity<Empty>();
			e.addGroup(Manager::groupDebugBoxEntities);

			e.addComponent<BoxComponent>();
			auto& c = e.addComponent<TransformComponent>();

			c.size = cell->boundingBox_size;
			c.position = cell->boundingBox_center;
		}

		auto addBoxFromEntity = [&](Entity* ent) {
			auto& e = manager->addEntity<Empty>();
			e.addGroup(Manager::groupDebugBoxEntities);

			auto& tr = ent->GetComponent<TransformComponent>();
			e.addComponent<BoxComponent>();

			auto& c = e.addComponent<TransformComponent>();
			c.size = tr.size;
			c.position = tr.position;

			return e.getId();
			};

		for (auto& group : {
	   manager->getVisibleGroup<NodeEntity>(Manager::groupNodes_0),
	   manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_0),
	   manager->getVisibleGroup<NodeEntity>(Manager::groupGroupNodes_1)
			})
		{
			for (auto id : group)
			{
				Entity* ent = manager->getEntityFromId(id);
				if (ent && ent->hasComponent<TransformComponent>())
					addBoxFromEntity(ent);
			}
		}
	}
	// 9.1. Debug Rendering (Boxes)
	{
		if (renderDebug) {
			Taz::GECSRenderBatch debugBatch;
			debugBatch.renderer_type = Taz::RenderBatch::RendererType::Line;
			debugBatch.mesh_type = Taz::RenderBatch::MeshType::Box;

			debugBatch.batchName = manager->getGroupName(Manager::groupDebugBoxEntities);
			debugBatch.shaderName = "lineColor";

			debugBatch.entities = manager->collectEntities({
				}, Taz::EntityType::Empty);
			debugBatch.count = debugBatch.entities.size();
			debugBatch.viewportSize =
				glm::vec2(
					getApp()->_window.getScreenWidth(),
					getApp()->_window.getScreenHeight()
				);
			frameData.batches.push_back(debugBatch);

		}
	}
	// 9.2. Debug Rendering (Rectangles)
	{
		if (renderDebug) {
			Taz::GECSRenderBatch debugBatch;

			debugBatch.renderer_type = Taz::RenderBatch::RendererType::Line;
			debugBatch.mesh_type = Taz::RenderBatch::MeshType::Quad;

			debugBatch.batchName = manager->getGroupName(Manager::groupDebugRectangleEntities);
			debugBatch.shaderName = "lineColor";

			debugBatch.entities = manager->collectEntities({
				}, Taz::EntityType::Empty);
			debugBatch.count = debugBatch.entities.size();
			debugBatch.viewportSize =
				glm::vec2(
					getApp()->_window.getScreenWidth(),
					getApp()->_window.getScreenHeight()
				);
			frameData.batches.push_back(debugBatch);

		}
	}

	minimapPrepareDraw();

	{
		for (auto& batch : frameData.batches) {
			getApp()->prepareBatch(batch);
		}
	}
	activeFrameIndex.store(writeIndex);
}

void Graph::renderDraw()
{
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	int readIndex = activeFrameIndex.load();
	auto& frameData = frameDataBuffers[readIndex];

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

	{
		//! render Frame
		for (const auto& batch : frameData.batches) {
			getApp()->renderBatch(batch, frameData, *main_camera2D);
		}
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glBindTexture(GL_TEXTURE_2D, 0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	_viewportFramebuffer.Unbind();

	minimapRenderDraw();
}


void Graph::minimapPrepareDraw() {
	int writeIndex = 1 - activeFrameIndex.load();

	auto& minimap_frameData = minimap_frameDataBuffers[writeIndex];

	auto& frameData = frameDataBuffers[writeIndex];
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

	minimap_frameData.batches.clear();
	{
		Taz::GECSRenderBatch minimapBatch;
		minimapBatch.renderer_type = Taz::RenderBatch::RendererType::PlaneColor;
		minimapBatch.mesh_type = Taz::RenderBatch::MeshType::Quad;

		minimapBatch.shaderName = "color";
		minimapBatch.entities = manager->collectEntities({
			Manager::groupMinimapNodes,
			}, Taz::EntityType::Minimap);
		minimapBatch.count = minimapBatch.entities.size();
		minimapBatch.rotationMatrix = minimap_rotationMatrix;
		minimap_frameData.batches.push_back(minimapBatch);
	}

	//! Prepare Draw Batches by Frame
	{
		for (auto& batch : minimap_frameData.batches) {
			getApp()->prepareBatch(batch);
		}
	}


}

void Graph::minimapRenderDraw() {
	int readIndex = activeFrameIndex.load();
	auto& minimap_frameData = minimap_frameDataBuffers[readIndex];

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
		for (const auto& batch : minimap_frameData.batches) {
			getApp()->renderBatch(batch, minimap_frameData, *minimap_camera2D);
		}
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glBindTexture(GL_TEXTURE_2D, 0);
	///////////////////////////////////////////////////////

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	_minimapFramebuffer.Unbind();
}