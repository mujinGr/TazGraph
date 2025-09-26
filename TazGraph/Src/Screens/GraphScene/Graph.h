#ifndef GRAPH_H
#define GRAPH_H

#include <AppScene/IScene.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>
#include <GLSLProgram.h>
#include <Renderers/ResourceManager/ResourceManager.h>
#undef main
#include <iostream>
#include <vector>
#include "Camera2.5D/PerspectiveCamera.h"
#include <Renderers/FrameBuffer/Framebuffer.h>
#include <Renderers/PlaneRenderers/PlaneModelRenderer/PlaneModelRenderer.h>
#include <InputManager/InputManager.h>
#include <BaseFPSLimiter/BaseFPSLimiter.h>
//#include "SpriteFont/SpriteFont.h"
#include "Window/Window.h"
#include "TextureManager/TextureManager.h"


#include "AppScene/ScreenIndices.h"

#include <ImGuiInterface/ImGuiInterface.h>

#include <chrono>
#include <thread>
#include "../../AssetManager/AssetManager.h"
#include "../../EditorIMGUI/GraphEditorLayer/GraphEditorLayer.h"

class Map;
class SceneManager;
class ColliderComponent;
class TransformComponent;

#define ON_HOVER 0
#define CTRLD_LEFT_CLICK -1
#define HOLD_TIME_FOR_SELECTION 1000

class Graph : public IScene {

public:
	Graph(TazGraphEngine::Window* window);
	~Graph();


	virtual int getNextSceneIndex() const override;

	virtual int getPreviousSceneIndex() const override;

	virtual void build() override;

	virtual void destroy() override;

	virtual void onEntry() override;

	virtual void onExit() override;

	virtual void update(float deltaTime) override;

	virtual void draw() override;

	void minimapDraw();

	virtual void BeginRender() override;
	virtual void updateUI(float deltaTime) override;
	virtual void drawUI() override;
	virtual void EndRender() override;


	/////////////////////////
	void renderBatch(const std::vector<LinkEntity*>& entities, LineRenderer& batch);
	void renderBatch(const std::vector<EmptyEntity*>& entities, PlaneColorRenderer& batch);
	void renderBatch(const std::vector<NodeEntity*>& entities, PlaneColorRenderer& batch);
	void renderBatch(const std::vector<EmptyEntity*>& entities, PlaneModelRenderer& batch);
	void renderBatch(const std::vector<NodeEntity*>& entities, PlaneModelRenderer& batch);
	void renderBatch(const std::vector<EmptyEntity*>& entities, LightRenderer& batch);
	void drawHUD(const std::vector<NodeEntity*>& entities);

	/////////////////////////

	Map* map = nullptr;
	//std::unique_ptr<Grid> grid;

	static TazGraphEngine::Window* _window;


private:
	float _backgroundColor[4] = { 0.407f,0.384f,0.356f, 1.0f };

	std::vector<Cell*> traversedCellsFromRay(glm::vec3 rayOrigin,
		glm::vec3 rayDirection,
		float maxDistance);

	void selectEntityFromRay(glm::vec3 rayOrigin, glm::vec3 rayDirection, int activateMode);

	bool setManager(std::string m_managerName) override;

	void checkInput();
	void performFrustumSelection();
	bool onPauseGraph();

	PlaneModelRenderer _PlaneModelRenderer;
	PlaneModelRenderer _hudPlaneModelRenderer;
	PlaneColorRenderer _PlaneColorRenderer;
	LineRenderer _LineRenderer;
	LightRenderer _LightRenderer;

	ResourceManager _resourceManager;

	std::vector<std::pair<Entity*, glm::vec3>> _selectedEntities;
	Entity* _displayedEntity = nullptr;
	bool _sceneManagerActive = false;
	Entity* _onHoverEntity = nullptr;

	Uint32 _holdStartTime = 0;

	bool _isDraggingSelectionBox = false;

	glm::vec2 _selectionStartPos = glm::vec2(0);
	glm::vec2 _selectionCurrentPos = glm::vec2(0);

	glm::vec2 _selectionWindowStartPos = glm::vec2(0);
	glm::vec2 _selectionWindowCurrentPos = glm::vec2(0);

	int _nextSceneIndex = SCENE_INDEX_GRAPHPLAY;
	int _prevSceneIndex = SCENE_INDEX_MAIN_MENU;

	const float SCALE_SPEED = 0.1f;
	bool _firstLoop = true;

	GraphEditorLayer _graphEditorLayer;

	Framebuffer _viewportFramebuffer;
	Framebuffer _minimapFramebuffer;

	ImVec2 _viewportPos;
	ImVec2 _viewportSize;

	glm::vec2 _viewportMousePosition = { 0.f,0.f };

	// Minimap stuff
	int currentX = 0, currentY = 0, currentZ = 0;
	bool processingComplete = false;
	bool needsRefresh = true;

	template<typename EntityType>
	void selectEntitiesInFrustum(int groupId, const SelectionFrustum& frustum);

};


#endif