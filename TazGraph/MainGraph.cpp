#include "MainGraph.h"
#include <iostream>
#include <string>
#include "ConsoleLogger/ConsoleLogger.h"
#include <IMGUI/imgui.h>

MainGraph::MainGraph()
{
	_window = nullptr;
	_screenWidth = 1024;
	_screenHeight = 780;
	_graphState = GraphState::PLAY;
}

MainGraph::~MainGraph()
{
}

void MainGraph::run()
{
	initSystems();

	_node = Node(-0.5f, -0.5f , 1.0f, 1.0f);
	_node.init();

	graphLoop();
}

void MainGraph::initSystems()
{
	// initialize SDL
	SDL_Init(SDL_INIT_EVERYTHING);

	_window = SDL_CreateWindow("Taz Graph", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _screenWidth, _screenHeight, SDL_WINDOW_OPENGL);

	if (_window == nullptr) {
		nmConsoleLogger::error("SDL window could not be created!");
	}

	SDL_GLContext glContext = SDL_GL_CreateContext(_window);
	if (glContext == nullptr) {
		nmConsoleLogger::error("SDL_GL context could not be created!");
	}

	GLenum error = glewInit();

	if (error != GLEW_OK) {
		nmConsoleLogger::error("Could not initialize glew");
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // whenever you clear the color bit it will get the value in the call

	initShaders();

}

void MainGraph::initShaders()
{
	_colorProgram.compileShaders("Shaders/colorShading.vert", "Shaders/colorShading.frag");
	_colorProgram.addAttribute("vertexPosition");
	_colorProgram.linkShaders();
}

void MainGraph::graphLoop()
{
	while (_graphState != GraphState::EXIT) {
		processInput();
		drawGraph();
		//updateUI();
	}

}

void MainGraph::processInput()
{
	SDL_Event e;

	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_QUIT:
			_graphState = GraphState::EXIT;
			break;

		case SDL_MOUSEMOTION:
			std::cout << e.motion.x << " " << e.motion.y << std::endl;
			break;


		}
	}
	
}

void MainGraph::drawGraph()
{
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the color and depth bits

	_colorProgram.use();

	_node.draw();

	_colorProgram.unuse();


	SDL_GL_SwapWindow(_window);
}

void MainGraph::updateUI() {
	// Default ImGui window
	//ImGui::Begin("Default UI");
	//ImGui::Text("This is a permanent UI element.");
	//ImGui::End();

}