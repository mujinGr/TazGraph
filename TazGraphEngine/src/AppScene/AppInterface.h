#pragma once

#include "../../pch.h"

#include "../InputManager/InputManager.h"
#include "../Window/Window.h"

#include "SceneList.h"
#include "../BaseFPSLimiter/BaseFPSLimiter.h"

#include "../AudioEngine/AudioEngine.h"

#include "../Threader/Threader.h"
#include "../Threader/RenderCommandQueue.h"


class AppInterface {
public:
	AppInterface(int threadCount, std::string openFile,
		double initialTimestamp,
		int initialStep);
	virtual ~AppInterface();

	void run();
	void RenderThreadFunc();
	void exitSimulator();

	virtual void onInit() = 0;
	virtual void addScenes() = 0;
	virtual void onExit() = 0;

	void onSDLEvent(SDL_Event& evnt);

	InputManager _inputManager;
	TazGraphEngine::Window _window;

	BaseFPSLimiter& getFPSLimiter() { return _limiter; }
	AudioEngine& getAudioEngine() { return _audioEngine; }

	Threader threadPool;
	std::string openFile;
	double initialTimestamp;
	int initialStep;

	RenderCommandQueue queues[2];
	std::atomic<int> activeIndex = 0;
	std::atomic<bool> frameReady = false;

	PlaneModelRenderer planeModelRenderer;
	PlaneColorRenderer planeColorRenderer;
	LineRenderer lineRenderer;
	LightRenderer lightRenderer;

	ResourceManager resourceManager;

	//void renderBatch(
	//	const Taz::RenderBatch& batch,
	//	const Taz::FrameRenderData& frameData
	//);

	//void drawLineBatch(
	//	const Taz::RenderBatch& batch,
	//	const Taz::FrameRenderData& frameData
	//);
	//void drawPlaneColorBatch(
	//	const Taz::RenderBatch& batch,
	//	const Taz::FrameRenderData& frameData
	//);
	//void drawPlaneModelBatch(
	//	const Taz::RenderBatch& batch,
	//	const Taz::FrameRenderData& frameData
	//);
	//void drawLightBatch(
	//	const Taz::RenderBatch& batch,
	//	const Taz::FrameRenderData& frameData
	//);



protected:
	virtual void checkInput();
	virtual void update(float deltaTime);
	virtual void draw();
	virtual void updateUI(float deltaTime);
	virtual void drawUI();

	bool init();
	void initRenderers();
	bool initSystems();

	BaseFPSLimiter _limiter;
	AudioEngine _audioEngine;

	std::unique_ptr<SceneList> _sceneList = nullptr;
	bool _isRunning = false;

	const float SCALE_SPEED = 0.1f;
};