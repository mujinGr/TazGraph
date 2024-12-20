#pragma once

#include "GraphScreen/IGraphScreen.h"
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
#include "BaseFPSLimiter/BaseFPSLimiter.h"
//#include "../SpriteFont/SpriteFont.h"
#include "AudioEngine/AudioEngine.h"
#include "Window/Window.h"
#include "TextureManager/TextureManager.h"

#include "GOS/GOSManager.h"

#include "GraphScreen/ScreenIndices.h"

class AssetManager;

class MainMenuScreen : public IGraphScreen {
public:
	MainMenuScreen(TazGraphEngine::Window* window);
	~MainMenuScreen();

    virtual int getNextScreenIndex() const override;

    virtual int getPreviousScreenIndex() const override;

    virtual void build() override;

    virtual void destroy() override;

    virtual void onEntry() override;

    virtual void onExit() override;

    virtual void update(float deltaTime) override;

    virtual void draw() override;

    virtual void updateUI() override;

    static SpriteBatch _spriteBatch;

    static AssetManager* assets;
    static float backgroundColor[4];

    void renderBatch(const std::vector<Entity*>& entities);

private:
    void checkInput();
    bool onStartSimulator();
    bool onResumeSimulator();
    void onExitSimulator();

    TazGraphEngine::Window* _window;

    ResourceManager _resourceManager;

    int _nextScreenIndex = SCREEN_INDEX_GRAPHPLAY;
    int _prevScreenIndex = SCREEN_INDEX_GRAPHPLAY;

};