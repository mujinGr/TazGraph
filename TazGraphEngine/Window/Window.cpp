#include "Window.h"
#include "../ConsoleLogger.h"


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

    //Open an SDL window
    _sdlWindow = SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _screenWidth, _screenHeight, flags);
    if (_sdlWindow == nullptr) {
        TazGraphEngine::ConsoleLogger::error("SDL Window could not be created!");
    }

    //Set up our OpenGL context
    SDL_GLContext glContext = SDL_GL_CreateContext(_sdlWindow);
    if (glContext == nullptr) {
        TazGraphEngine::ConsoleLogger::error("SDL_GL context could not be created!");
    }

    //Set up glew (optional but recommended)
    GLenum error = glewInit();
    if (error != GLEW_OK) {
        TazGraphEngine::ConsoleLogger::error("Could not initialize glew!");
    }

    ImGui::CreateContext();
    ImPlot::CreateContext();
    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(_sdlWindow, &glContext);
    ImGui_ImplOpenGL3_Init("#version 330"); // Or whatever GLSL version suits your needs

    //Check the OpenGL version
    std::cout << "***   OpenGL Version: "<< glGetString(GL_VERSION) << "    ***\n";

    //Set VSYNC
    SDL_GL_SetSwapInterval(0);

    // Enable alpha blend
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return 0;
}

void TazGraphEngine::Window::swapBuffer() {
    SDL_GL_SwapWindow(_sdlWindow);
}

