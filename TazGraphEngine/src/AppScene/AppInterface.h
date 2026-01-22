#pragma once

#include "../../pch.h"

#include "../InputManager/InputManager.h"
#include "../Window/Window.h"

#include "SceneList.h"
#include "../BaseFPSLimiter/BaseFPSLimiter.h"

#include "../AudioEngine/AudioEngine.h"

#include "../Threader/Threader.h"
#include "../Threader/RenderCommandQueue.h"

#define BUFFERS_NO 2



class AppInterface {
public:
	AppInterface(int threadCount, int msaa_samples, std::string openFile,
		double initialTimestamp,
		int initialStep,
		bool usePython,
		std::array<float, 4> bg,
		bool useGrid);
	virtual ~AppInterface();

	void run();

	void enqueueRenderCommand(std::function<void()> cmd);
	void waitForRenderCommand();

	void waitForRenderThreadExit();

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
	int MSAA_samples;
	bool useMSAA = false;
	std::string openFile;
	double initialTimestamp;
	int initialStep;
	bool usePython = false;
	std::array<float, 4> backgroundColor;
	bool useGrid = false;

	std::condition_variable initCommandCV;

	RenderCommandQueue queues[BUFFERS_NO];
	std::atomic<int> activeIndex = 0;

	RenderCommandQueue initQueue;
	std::mutex initMutex;
	std::condition_variable initCV;
	std::atomic<bool> initCommandReady{ false };
	std::atomic<bool> initCommandComplete{ false };

	std::vector<SDL_Event> imguiEvents;
	std::mutex imguiEventsMutex;

	ResourceManager resourceManager;

	void renderBatch(
		const Taz::GECSRenderBatch& batch,
		Taz::FrameRenderData& frameData,
		ICamera& camera
	);

	void drawLineBatch(
		const Taz::GECSRenderBatch& batch,
		Taz::FrameRenderData& frameData,
		ICamera& camera
	);
	void drawPlaneColorBatch(
		const Taz::GECSRenderBatch& batch,
		Taz::FrameRenderData& frameData,
		ICamera& camera
	);
	void drawPlaneModelBatch(
		const Taz::GECSRenderBatch& batch,
		Taz::FrameRenderData& frameData,
		ICamera& camera
	);
	void drawLightBatch(
		const Taz::GECSRenderBatch& batch,
		Taz::FrameRenderData& frameData,
		ICamera& camera
	);

	void drawBatch(const std::vector<Entity*>& entities, LineRenderer& batch);
	void drawBatch(const std::vector<Entity*>& entities, PlaneColorRenderer& batch);
	void drawBatch(const std::vector<Entity*>& entities, PlaneModelRenderer& batch);
	void drawBatch(const std::vector<Entity*>& entities, LightRenderer& batch);

	void prepareBatch(Taz::GECSRenderBatch& batch, Taz::FrameRenderData& frameData);

	void prepareLineBatch(Taz::GECSRenderBatch& batch, Taz::FrameRenderData& frameData);
	void preparePlaneColorBatch(Taz::GECSRenderBatch& batch, Taz::FrameRenderData& frameData);
	void preparePlaneModelBatch(Taz::GECSRenderBatch& batch, Taz::FrameRenderData& frameData);
	void prepareLightBatch(Taz::GECSRenderBatch& batch, Taz::FrameRenderData& frameData);


protected:
	virtual void checkInput();
	virtual void update(float deltaTime);

	virtual void prepareDraw(int index);
	virtual void renderDraw(int index);

	virtual void updateUI(float deltaTime);
	virtual void drawUI();

	void disposeRenderers(int index);

	virtual void swapBuffer();

	bool init();
	void initRenderers();
	bool initSystems();

	BaseFPSLimiter _limiter;
	AudioEngine _audioEngine;

	std::unique_ptr<SceneList> _sceneList = nullptr;
	bool _isRunning = false;

	const float SCALE_SPEED = 0.1f;

	std::thread renderThread;
};