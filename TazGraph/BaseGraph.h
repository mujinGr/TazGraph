#pragma once

#include <numeric>
#include <SDL/SDL.h>
#include <GL/glew.h>
#include "Window/Window.h"

#include "GraphSystem/Node.h"
#include "GLSLProgram/GLSLProgram.h"
#include "BaseFPSLimiter/BaseFPSLimiter.h"


enum GraphState {
	PLAY,
	EXIT
};

class BaseGraph
{

public:
	BaseGraph();
	~BaseGraph();

	void run();


private:
	bool initSystems();
	void initShaders();

	void graphLoop();
	void update(float deltaTime);
	void processInput();

	void drawGraph();

	void updateUI();

	TazGraph::Window _window;

	GraphState _graphState;

	BaseFPSLimiter _limiter;

	Node _node; //  test node
	Node _node2; //  test node


	GLSLProgram _colorProgram;
};

