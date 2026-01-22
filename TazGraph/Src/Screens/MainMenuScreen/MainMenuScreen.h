#pragma once

#include "TazGraphEngine.h"
#undef main
//#include "../SpriteFont/SpriteFont.h"
#include "../../EditorIMGUI/MainMenuEditorLayer/MainMenuEditorLayer.h"
#include "../../AssetManager/AssetManager.h"

class MainMenuScreen : public IScene {
public:
	MainMenuScreen();
	~MainMenuScreen();

	virtual int getNextSceneIndex() const override;

	virtual int getPreviousSceneIndex() const override;

	virtual void build() override;

	virtual void destroy() override;

	virtual void onEntry() override;

	virtual void onExit() override;

	virtual void update(float deltaTime) override;

	virtual void prepareDraw(int index) override;
	virtual void renderDraw(int index) override;

	virtual void BeginRender() override;
	virtual void updateUI(float deltaTime) override;
	virtual void drawUI() override;
	virtual void SwapBufferDraw() override;
	virtual void EndRender() override;

private:

	void checkInput();
	bool onStartSimulator();
	bool onResumeSimulator();
	void onExitSimulator();

	int _nextSceneIndex = SCENE_INDEX_GRAPHPLAY;
	int _prevSceneIndex = SCENE_INDEX_GRAPHPLAY;

	MainMenuEditorLayer _mainMenuLayer;
};