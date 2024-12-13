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
#include "SpriteBatch/SpriteBatch.h"
#include "InputManager/InputManager.h"
#include "Timing/Timing.h"
//#include "SpriteFont/SpriteFont.h"
#include "AudioEngine/AudioEngine.h"
#include "Window/Window.h"
#include "TextureManager/TextureManager.h"

#include "ECS/ECSManager.h"

#include "GraphScreen/ScreenIndices.h"

#include "Grid/Grid.h"

class Map;
class AssetManager;
class SceneManager;
class ColliderComponent;
class TransformComponent;

constexpr int CELL_SIZE = 128;

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

	virtual void updateUI() override;



	/////////////////////////
	
	void renderBatch(const std::vector<Entity*>& entities, SpriteBatch& batch);
	void drawHUD(const std::vector<Entity*>& entities, const std::string& textureName);

	/////////////////////////

	static SDL_Event event;

	static SpriteBatch _spriteBatch;
	static SpriteBatch _hudSpriteBatch;

	static AudioEngine audioEngine;

	static Map* map;

	static AssetManager* assets;

	static SceneManager* scenes;

	//std::unique_ptr<Grid> grid;

	static TazGraphEngine::Window* _window;
	float startTime = SDL_GetTicks() / 1000.0f;

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
};


#endif