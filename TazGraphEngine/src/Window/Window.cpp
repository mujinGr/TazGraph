#include "Window.h"
#include <SDL2/SDL_image.h>

TazGraphEngine::Window::Window()
{
}


TazGraphEngine::Window::~Window()
{
}

int TazGraphEngine::Window::create(std::string windowName, int screenWidth, int screenHeight, float scale, unsigned int currentFlags) {

	//Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
	Uint32 flags = (1 << 1) | (1 << 5) | (1 << 7);
	_screenWidth = screenWidth * scale;
	_screenHeight = screenHeight * scale;
	_scale = scale;

	if (currentFlags & INVISIBLE) {
		flags |= SDL_WINDOW_HIDDEN;
	}
	if (currentFlags & VISIBLE) {
		flags &= ~INVISIBLE;
	}
	if (currentFlags & FULLSCREEN) {
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	}
	if (currentFlags & BORDERLESS) {
		flags |= SDL_WINDOW_BORDERLESS;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); //! BUFFERS
	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);//! SAMPLES (request)
	//Open an SDL window
	_sdlWindow = SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _screenWidth, _screenHeight, flags);
	if (_sdlWindow == nullptr) {
		TazGraphEngine::ConsoleLogger::error("SDL Window could not be created!");
	}
	SDL_Surface* icon = IMG_Load("../TazGraph/assets/Sprites/TazGraph_whitebg.jpg");
	SDL_SetWindowIcon(_sdlWindow, icon);
	//Set up our OpenGL context
	glContext = SDL_GL_CreateContext(_sdlWindow);
	if (glContext == nullptr) {
		TazGraphEngine::ConsoleLogger::error("SDL_GL context could not be created!");
	}
	SDL_GL_MakeCurrent(_sdlWindow, glContext);

	//Set up glew (optional but recommended)
	GLenum error = glewInit();
	if (error != GLEW_OK) {
		TazGraphEngine::ConsoleLogger::error("Could not initialize glew!");
	}
	int buffers, samples;
	//SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &buffers);
	//SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &samples);
	//std::cout << "*** Multisample Buffers: " << buffers << " ***\n";
	//std::cout << "*** Multisample Samples: " << samples << " ***\n";
	// this is setup of Imgui
	ImGui::CreateContext();
	ImPlot::CreateContext();
	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(_sdlWindow, &glContext);
	ImGui_ImplOpenGL3_Init("#version 430"); // Or whatever GLSL version suits your needs

	//Check the OpenGL version
	//std::cout << "***   OpenGL Version: " << glGetString(GL_VERSION) << "    ***\n";
	//Set VSYNC
	SDL_GL_SetSwapInterval(0);
	int interval = SDL_GL_GetSwapInterval();
	std::cout << "VSync: " << (interval == 0 ? "OFF" : "ON") << std::endl;
	// Enable alpha blend
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	SDL_GL_MakeCurrent(_sdlWindow, nullptr);

	return 0;
}

void TazGraphEngine::Window::swapBuffer() {
	SDL_GL_SwapWindow(_sdlWindow);
}

