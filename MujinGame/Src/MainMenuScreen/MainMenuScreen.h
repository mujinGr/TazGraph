#pragma once

#include "GameScreen/IGameScreen.h"
#include <SDL/SDL.h>
#include <SDL_IMAGE/SDL_image.h>
#include <GL/glew.h>
#include "GLSLProgram.h"
#undef main
#include <iostream>
#include <vector>
#include "Camera2.5D/PerspectiveCamera.h"
#include "SpriteBatch/SpriteBatch.h"
#include "InputManager/InputManager.h"
#include "Timing/Timing.h"
//#include "../SpriteFont/SpriteFont.h"
#include "AudioEngine/AudioEngine.h"
#include "Window/Window.h"
#include "TextureManager/TextureManager.h"

#include "ECS/ECSManager.h"

#include "GameScreen/ScreenIndices.h"

class AssetManager;

class MainMenuScreen : public IGameScreen {
public:
	MainMenuScreen(MujinEngine::Window* window);
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

    void setupShaderAndTexture(const std::string& textureName);
    void renderBatch(const std::vector<Entity*>& entities);

private:
    void checkInput();
    bool onStartGame();
    bool onResumeGame();
    void onExitGame();

    MujinEngine::Window* _window;

    GLSLProgram _colorProgram;
    GLSLProgram _textureProgram;

    int _nextScreenIndex = SCREEN_INDEX_GAMEPLAY;
    int _prevScreenIndex = SCREEN_INDEX_GAMEPLAY;

};