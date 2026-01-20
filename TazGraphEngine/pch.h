// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef TAZ_ENGINE_PCH_H
#define TAZ_ENGINE_PCH_H

#include "./Base.h"

#include <chrono>

#include <optional>
#include <variant>

#include <algorithm>
#include <bitset>
#include <array>
#include <unordered_map>

#include <thread>

#include <cmath>

#include <filesystem>
#include <typeindex>
#include <utility> 
#include <type_traits>
#include <sstream>
#include <unordered_set>
#include <list>

// Network
#if defined(TAZ_PLATFORM_LINUX) 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
using socket_t = int;
#define close_socket close
#endif
#if defined(TAZ_PLATFORM_WINDOWS)
#pragma comment(lib, "ws2_32.lib")
using socket_t = SOCKET;
#define close_socket closesocket
#endif

// SDL
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_rect.h>

// IMGUI
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>
#include <implot_internal.h>

#include <GL/glew.h>

// IMGUI
#include "ImGuiText.h"

#include <ImGuizmo/ImGuizmo.h>

// CUSTOM
#include <ImGuiComboAutoselect/imgui_combo_autoselect.h>

#endif //PCH_H
