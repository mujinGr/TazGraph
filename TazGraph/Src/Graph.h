#ifndef GRAPH_H
#define GRAPH_H

#include <GraphScreen/IGraphScreen.h>
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
#include "AudioEngine/AudioEngine.h"
#include "Window/Window.h"
#include "TextureManager/TextureManager.h"

#include "GOS/GOSManager.h"

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

class Graph : public IGraphScreen {

public:
	Graph(TazGraphEngine::Window* window);
	~Graph();


	virtual int getNextScreenIndex() const override;

	virtual int getPreviousScreenIndex() const override;

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

	static SDL_Event event;

	static PlaneModelRenderer _PlaneModelRenderer;
	static PlaneModelRenderer _hudPlaneModelRenderer;

	static AudioEngine audioEngine;

	static Map* map;

	static AssetManager* assets;

	static SceneManager* scenes;

	//std::unique_ptr<Grid> grid;

	static TazGraphEngine::Window* _window;

	static float backgroundColor[4];

private:
	void selectEntityAtPosition(glm::vec2 worldCoords);
	void checkInput();
	bool onPauseGraph();

	ResourceManager _resourceManager;

	//ResourceManager _resourceManager

	Entity* _selectedEntity;
	
	Grid* _grid = nullptr; //< Grid for spatial partitioning for collision

	int _nextScreenIndex = SCREEN_INDEX_GRAPHPLAY;
	int _prevScreenIndex = SCREEN_INDEX_GRAPHPLAY;

	const float SCALE_SPEED = 0.1f;
	bool _firstLoop = true;

	EditorIMGUI _editorImgui;

	bool _showSaveWindow = false;

};


#endif