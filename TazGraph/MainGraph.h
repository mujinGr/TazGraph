#pragma once


#include <SDL/SDL.h>
#include <GL/glew.h>

#include "GraphSystem/Node.h"
#include "GLSLProgram/GLSLProgram.h"

enum GraphState {
	PLAY,
	EXIT
};

class MainGraph
{
public:
	MainGraph();
	~MainGraph();

	void run();


private:
	void initSystems();
	void initShaders();
	void graphLoop();
	void processInput();
	void drawGraph();

	SDL_Window* _window;
	int _screenWidth, _screenHeight;

	GraphState _graphState;

	Node _node; //  test node

	GLSLProgram _colorProgram;
};

