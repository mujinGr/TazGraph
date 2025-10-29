// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H


#include <iostream>
#include <string>
#include <ctime>

#include <unordered_map>
#include <memory>
#include <map>
#include <vector>
#include <fstream>

// add headers that you want to pre-compile here
//#include "framework.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/rotate_vector.hpp>
#include "glm/gtc/type_ptr.hpp"

#include <imgui.h>

#include <SDL2/SDL.h>

#endif //PCH_H
