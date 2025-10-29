#pragma once

#include "TazGraphEngine.h"
#undef main
//#include "../SpriteFont/SpriteFont.h"
#include "../../EditorIMGUI/MainMenuEditorLayer/MainMenuEditorLayer.h"
#include "../../AssetManager/AssetManager.h"

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
	virtual void updateUI(float deltaTime) override;
	virtual void drawUI() override;
	virtual void EndRender() override;


	void renderBatch(const std::vector<EntityID>& entities);

private:

	void checkInput();
	bool onStartSimulator();
	bool onResumeSimulator();
	void onExitSimulator();

	TazGraphEngine::Window* _window;

	PlaneModelRenderer _PlaneModelRenderer;

	ResourceManager _resourceManager;

	int _nextSceneIndex = SCENE_INDEX_GRAPHPLAY;
	int _prevSceneIndex = SCENE_INDEX_GRAPHPLAY;

	MainMenuEditorLayer _mainMenuLayer;
};