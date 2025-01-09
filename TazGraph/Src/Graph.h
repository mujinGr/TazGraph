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
#include "Renderers/PlaneModelRenderer/PlaneModelRenderer.h"
#include "InputManager/InputManager.h"
#include "BaseFPSLimiter/BaseFPSLimiter.h"
//#include "SpriteFont/SpriteFont.h"
#include "Window/Window.h"
#include "TextureManager/TextureManager.h"

#include "GECS/GECSManager.h"

#include "GraphScreen/ScreenIndices.h"

#include "Grid/Grid.h"
#include "EditorIMGUI/EditorIMGUI.h"

class Map;
class AssetManager;
class SceneManager;
class ColliderComponent;
class TransformComponent;

constexpr int CELL_SIZE = 64;
constexpr int AXIS_CELLS = 100;
constexpr int ROW_CELL_SIZE = AXIS_CELLS * CELL_SIZE;
constexpr int COLUMN_CELL_SIZE = AXIS_CELLS * CELL_SIZE;

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
	void renderBatch(const std::vector<Entity*>& entities, LineRenderer& batch);
	void renderBatch(const std::vector<Entity*>& entities, PlaneModelRenderer& batch);
	void drawHUD(const std::vector<Entity*>& entities, const std::string& textureName);

	/////////////////////////

	

	static Map* map;

	//std::unique_ptr<Grid> grid;

	static TazGraphEngine::Window* _window;

private:
	float _backgroundColor[4] = { 0.78f,0.88f,1.f, 1.0f };

	void selectEntityAtPosition(glm::vec2 worldCoords);
	void checkInput();
	bool onPauseGraph();

	PlaneModelRenderer _PlaneModelRenderer;
	PlaneModelRenderer _hudPlaneModelRenderer;

	AssetManager* _assetsManager;

	ResourceManager _resourceManager;

	//ResourceManager _resourceManager

	Entity* _selectedEntity;
	
	Grid* _grid = nullptr; //< Grid for spatial partitioning for collision

	int _nextSceneIndex = SCENE_INDEX_GRAPHPLAY;
	int _prevSceneIndex = SCENE_INDEX_MAIN_MENU;

	const float SCALE_SPEED = 0.1f;
	bool _firstLoop = true;

	EditorIMGUI _editorImgui;

	bool _showSaveWindow = false;

};


#endif