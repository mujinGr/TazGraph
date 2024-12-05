#include "BaseGraph.h"
#include <iostream>
#include <string>
#include <algorithm>
#include "ConsoleLogger/ConsoleLogger.h"
#include <IMGUI/imgui.h>
#include <IMGUI/imgui_impl_sdl2.h>
#include <IMGUI/imgui_impl_opengl3.h>
#include "BaseFPSLimiter/BaseFPSLimiter.h"

BaseGraph::BaseGraph()
{
	_graphState = GraphState::PLAY;
}

BaseGraph::~BaseGraph()
{
}

void BaseGraph::run()
{
	if(!initSystems()) return;

	BaseFPSLimiter limiter;
	limiter.setMaxFPS(60.0f);

	// here is main graph init
	_node = Node(-0.5f, -0.5f , 0.5f, 0.5f);
	_node.init();

	_node2 = Node(0.5f, 0.5f, 0.5f, 0.5f);
	_node2.init();

	graphLoop();
}

bool BaseGraph::initSystems()
{
	// initialize SDL
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	_window.create("Taz Graph", 800, 640, 1.0f, TazGraph::VISIBLE);

	initShaders();

	return true;
}

void BaseGraph::initShaders()
{
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	_colorProgram.compileShaders("Shaders/colorShading.vert", "Shaders/colorShading.frag");
	_colorProgram.addAttribute("vertexPosition");
	_colorProgram.addAttribute("vertexColor");
	//_colorProgram.addAttribute("vertexUV");
	_colorProgram.linkShaders();
}

void BaseGraph::graphLoop()
{
	const float DESIRED_FPS = 60;
	const int MAX_PHYSICS_STEPS = 6;

	const float MS_PER_SECOND = 1000;
	const float DESIRED_FRAMETIME = MS_PER_SECOND / DESIRED_FPS;
	const float MAX_DELTA_TIME = 1.0f;

	float prevTicks = SDL_GetTicks();
	_limiter.setMaxFPS(60.0f);


	while (_graphState != GraphState::EXIT) {
		_limiter.begin();

		processInput();

		float newTicks = SDL_GetTicks();
		float frameTime = newTicks - prevTicks;
		prevTicks = newTicks;
		float totalDeltaTime = frameTime / DESIRED_FRAMETIME;


		int i = 0;
		while (totalDeltaTime > 0.0f && i < MAX_PHYSICS_STEPS)
		{
			float deltaTime = std::min(totalDeltaTime, MAX_DELTA_TIME);
			update(deltaTime); //handleEvents first thing in update
			totalDeltaTime -= deltaTime;
			i++;
			if (_graphState == GraphState::PLAY) {
				drawGraph();
			}
		}

		_limiter.fps = _limiter.end();

		static int frameCounter = 0;
		frameCounter++;
		if (frameCounter == 10) {
			_limiter.setHistoryValue(_limiter.fps);
			

			std::cout << "FPS: " << _limiter.fps << std::endl;
			frameCounter = 0;

		}
		updateUI();
		_window.swapBuffer();
	}

}

void BaseGraph::update(float deltaTime)
{

}

void BaseGraph::processInput()
{
	SDL_Event e;

	while (SDL_PollEvent(&e)) {
		ImGui_ImplSDL2_ProcessEvent(&e);
		switch (e.type) {
		case SDL_QUIT:
			_graphState = GraphState::EXIT;
			break;

		case SDL_MOUSEMOTION:
			std::cout << "Mouse Coords: " << e.motion.x << " " << e.motion.y << std::endl;
			break;
		}
	}
	
}

void BaseGraph::drawGraph()
{
	glClearColor(0, 0, 255, 255);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the color and depth bits

	_colorProgram.use();

	_node.draw();

	_colorProgram.unuse();

	_colorProgram.use();

	_node2.draw();

	_colorProgram.unuse();

}

void BaseGraph::updateUI() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	//ImGui::ShowDemoWindow(); //uncommend this if there are any issues with GUI

	// Default ImGui window
	ImGui::Begin("Default UI");
	ImGui::Text("This is a permanent UI element.");
	ImGui::End();

	ImGui::Begin("Performance");
	if (ImPlot::BeginSubplots("", 1, 1, ImVec2(400, 300)))
	{
		if (ImPlot::BeginPlot(""))
		{
			ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 5.f);

			ImPlot::SetupAxis(ImAxis_X1, "Frame Number", ImPlotAxisFlags_AutoFit);
			ImPlot::SetupAxis(ImAxis_Y1, "Frame Rate");
			ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100);

			std::vector<float> local_samples(_limiter.FPS_HISTORY_COUNT);
			std::iota(local_samples.begin(), local_samples.end(), 0);

			std::vector<float> fps_data(_limiter.fpsHistory.begin(), _limiter.fpsHistory.end());

			ImPlot::PlotLine("Frame Rate", local_samples.data(), fps_data.data(), fps_data.size());
			ImPlot::EndPlot();

		}
		ImPlot::EndSubplots();
	}

	ImGui::End();
	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}