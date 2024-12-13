message("\nInside TazGraphEngine Cmake...\n")

set(PROJECT_NAME TazGraphEngine)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "AudioEngine/AudioEngine.h"
    "Camera2.5D/CameraManager.h"
    "Camera2.5D/ICamera.h"
    "Camera2.5D/PerspectiveCamera.h"
    "DebugRenderer/DebugRenderer.h"
    "ECS/Animators/Animation.h"
    "ECS/Animators/AnimatorComponent.h"
    "ECS/Animators/AnimatorManager.h"
    "ECS/Animators/FlashAnimation.h"
    "ECS/Animators/FlashAnimatorComponent.h"
    "ECS/ButtonComponent.h"
    "ECS/ColliderComponent.h"
    "ECS/Components.h"
    "ECS/ECS.h"
    "ECS/ECSManager.h"
    "ECS/GridComponent.h"
    "ECS/LightComponent.h"
    "ECS/LightTextureComponent.h"
    "ECS/Rectangle_w_Color.h"
    "ECS/RigidBodyComponent.h"
    "ECS/SpriteComponent.h"
    "ECS/TileComponent.h"
    "ECS/TransformComponent.h"
    "GLSLProgram.h"
    "GLTexture.h"
    "GraphScreen/IGraphScreen.h"
    "GraphScreen/IMainGraph.h"
    "GraphScreen/ScreenList.h"
    "Grid/Grid.h"
    "InputManager/InputManager.h"
    "picoPNG.h"
    "PNG_Letters.h"
    "ResourceManager/ResourceManager.h"
    "SceneManager/SceneManager.h"
    "SpriteBatch/SpriteBatch.h"
    "TextureManager/TextureManager.h"
    "Timing/Timing.h"
    "Vertex.h"
    "Window/Window.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "AudioEngine/AudioEngine.cpp"
    "Camera2.5D/OrthoCamera.h"
    "DebugRenderer/DebugRenderer.cpp"
    "ECS/ECS.cpp"
    "GraphScreen/IGraphScreen.cpp"
    "GraphScreen/IMainGraph.cpp"
    "GraphScreen/ScreenIndices.h"
    "GraphScreen/ScreenList.cpp"
    "Grid/Grid.cpp"
    "imgui/imgui.cpp"
    "imgui/imgui_demo.cpp"
    "imgui/imgui_draw.cpp"
    "imgui/imgui_impl_opengl3.cpp"
    "imgui/imgui_impl_sdl2.cpp"
    "imgui/imgui_tables.cpp"
    "imgui/imgui_widgets.cpp"
    "InputManager/InputManager.cpp"
    "PNG_Letters.cpp"
    "ResourceManager/ResourceManager.cpp"
    "SceneManager/SceneManager.cpp"
    "SpriteBatch/SpriteBatch.cpp"
    "TextureManager/TextureManager.cpp"
    "Timing/Timing.cpp"
    "Window/Window.cpp"
)
source_group("Source Files" FILES ${Source_Files})

################################################################################
# Setting libs with local folder
################################################################################
#set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/deps/include")
#set(LIB_DIR "${PROJECT_SOURCE_DIR}/deps/lib")

#message("project source dir: ${PROJECT_SOURCE_DIR}")
#message("deps include dir: ${INCLUDE_DIR}")

#include_directories(${INCLUDE_DIR})
#link_directories(${LIB_DIR})

################################################################################
# Setting libs with find_package
################################################################################
# Find SDL2 core library
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Find SDL2 Mixer library
find_package(SDL2_mixer REQUIRED)
include_directories(${SDL2_MIXER_INCLUDE_DIRS})

# Set the local include of IMGUI
set(IMGUI_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/deps/include/IMGUI")
include_directories(${IMGUI_INCLUDE_DIR})

#file(GLOB IMGUI_SOURCES ${PROJECT_NAME}/imgui/*.cpp)
#target_sources(${PROJECT_NAME} PRIVATE ${IMGUI_SOURCES})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE TazGraphEngine)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "WIN32;"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /DEBUG;
                /OPT:REF;
                /OPT:ICF
            >
            /SUBSYSTEM:CONSOLE
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE
        )
    endif()
endif()

################################################################################
# Dependencies
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:Debug>:"
            "SDL2;"
            "SDL2main;"
            "SDL2_image;"
            "SDL2_ttf;"
            "SDL2_mixer;"
            "opengl32;"
            "glew32;"
            "Winmm"
        ">"
    )
endif()

message(" additional lib dependencies: ${ADDITIONAL_LIBRARY_DEPENDENCIES}")

target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

