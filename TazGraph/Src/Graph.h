#ifndef GRAPH_H
#define GRAPH_H

#include <GraphScreen/IScene.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>
#include "GLSLProgram.h"
#include "ResourceManager/ResourceManager.h"
#undef main
#include <iostream>
#include <vector>
#include "Camera2.5D/PerspectiveCamera.h"
#include <Renderers/FrameBuffer/Framebuffer.h>
#include "Renderers/PlaneModelRenderer/PlaneModelRenderer.h"
#include "InputManager/InputManager.h"
#include "BaseFPSLimiter/BaseFPSLimiter.h"
//#include "SpriteFont/SpriteFont.h"
#include "Window/Window.h"
#include "TextureManager/TextureManager.h"

#include "GECS/Core/GECSManager.h"

#include "GraphScreen/ScreenIndices.h"

#include "EditorIMGUI/EditorIMGUI.h"

#include <chrono>
#include <thread>

class Map;
class AssetManager;
class SceneManager;
class ColliderComponent;
class TransformComponent;

constexpr int CELL_SIZE = 100;
constexpr int AXIS_CELLS = 100;
constexpr int DEPTH_AXIS_CELLS = 3;
constexpr int ROW_CELL_SIZE		= AXIS_CELLS * CELL_SIZE;
constexpr int COLUMN_CELL_SIZE	= AXIS_CELLS * CELL_SIZE;
constexpr int DEPTH_CELL_SIZE	= DEPTH_AXIS_CELLS * CELL_SIZE;


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

	virtual void BeginRender() override;
	virtual void updateUI() override;
	virtual void EndRender() override;


	/////////////////////////
	void renderBatch(const std::vector<LinkEntity*>& entities, LineRenderer& batch);
	void renderBatch(const std::vector<EmptyEntity*>& entities, PlaneColorRenderer& batch, bool isTriangles);
	void renderBatch(const std::vector<NodeEntity*>& entities, PlaneColorRenderer& batch, bool isTriangles);
	void renderBatch(const std::vector<EmptyEntity*>& entities, PlaneModelRenderer& batch, bool isTriangles);
	void renderBatch(const std::vector<NodeEntity*>& entities, PlaneModelRenderer& batch, bool isTriangles);
	void drawHUD(const std::vector<NodeEntity*>& entities, const std::string& textureName);

	/////////////////////////

	

	static Map* map;

	//std::unique_ptr<Grid> grid;

	static TazGraphEngine::Window* _window;

private:
	float _backgroundColor[4] = { 0.78f,0.88f,1.f, 1.0f };

	std::vector<Cell*> traversedCellsFromRay(glm::vec3 rayOrigin,
		glm::vec3 rayDirection,
		float maxDistance);

	void selectEntityFromRay(glm::vec3 rayOrigin, glm::vec3 rayDirection, int activateMode);

	void selectEntityAtPosition(glm::vec2 worldCoords, int activateMode);
	void checkInput();
	bool onPauseGraph();

	PlaneModelRenderer _PlaneModelRenderer;
	PlaneModelRenderer _hudPlaneModelRenderer;
	PlaneColorRenderer _PlaneColorRenderer;
	LineRenderer _LineRenderer;

	AssetManager* _assetsManager;

	ResourceManager _resourceManager;

	Entity* _selectedEntity = nullptr;
	Entity* _displayedEntity = nullptr;
	
	int _nextSceneIndex = SCENE_INDEX_GRAPHPLAY;
	int _prevSceneIndex = SCENE_INDEX_MAIN_MENU;

	const float SCALE_SPEED = 0.1f;
	bool _firstLoop = true;

	EditorIMGUI _editorImgui;

	bool _showSaveWindow = false;

	Framebuffer _framebuffer;

	unsigned int _rectVAO, _rectVBO;

	unsigned int _FBO;
	unsigned int _framebufferTexture;
	unsigned int _RBO;

	ImVec2 _windowPos;
	ImVec2 _windowSize;

	glm::vec2 _sceneMousePosition = {0.f,0.f};
	glm::vec2 _savedMainViewportMousePosition = { 0.f,0.f };

	Threader threadPool = Threader(10);
};


#endif