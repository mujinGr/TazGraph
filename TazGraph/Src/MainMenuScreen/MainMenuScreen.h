#pragma once

#include "GraphScreen/IScene.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>
#include "GLSLProgram.h"
#include "ResourceManager/ResourceManager.h"
#undef main
#include <iostream>
#include <vector>
#include <functional>
#include "Camera2.5D/PerspectiveCamera.h"
#include "Renderers/PlaneModelRenderer/PlaneModelRenderer.h"
#include "InputManager/InputManager.h"
#include "BaseFPSLimiter/BaseFPSLimiter.h"
//#include "../SpriteFont/SpriteFont.h"
#include "Window/Window.h"
#include "TextureManager/TextureManager.h"

#include "GECS/Core/GECSManager.h"

#include "GraphScreen/ScreenIndices.h"

#include "../EditorIMGUI/EditorIMGUI.h"

class AssetManager;

class MainMenuScreen : public IScene {
public:
	MainMenuScreen(TazGraphEngine::Window* window);
	~MainMenuScreen();

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


    void renderBatch(const std::vector<Entity*>& entities);

private:
    float _backgroundColor[4] = { 0.8f, 0.8f, 0.8f, 1.0f };

    AssetManager* _assetsManager;

    void checkInput();
    bool onStartSimulator();
    bool onResumeSimulator();
    bool onLoadSimulator();
    void onExitSimulator();

    TazGraphEngine::Window* _window;

    PlaneModelRenderer _PlaneModelRenderer;

    ResourceManager _resourceManager;

    int _nextSceneIndex = SCENE_INDEX_GRAPHPLAY;
    int _prevSceneIndex = SCENE_INDEX_GRAPHPLAY;

    EditorIMGUI _editorImgui;
};