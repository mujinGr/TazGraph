#pragma once

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <string>

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>

namespace TazGraphEngine {

    enum WindowFlags { INVISIBLE = 0x1, VISIBLE = 0x2, FULLSCREEN = 0x4, BORDERLESS = 0x8 };

    class Window
    {
    public:
        Window();
        ~Window();

        int create(std::string windowName, int screenWidth, int screenHeight, float scale, unsigned int currentFlags);

        void swapBuffer();

        void setScreenWidth(int width) { _screenWidth = width; }
        int getScreenWidth() { return _screenWidth; }
        void setScreenHeight(int height) { _screenHeight = height; }
        int getScreenHeight() { return _screenHeight; }
        void setScale(float scale) { _scale = scale; }
        float getScale() { return _scale; }
    private:
        SDL_Window* _sdlWindow;
        int _screenWidth, _screenHeight;
        float _scale;
    };

}