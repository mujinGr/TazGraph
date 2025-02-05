message("\nInside TazGraphEngine Cmake...\n")

set(PROJECT_NAME TazGraphEngine)
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -z noexGGECStack")
################################################################################
# Source groups
################################################################################
set(Header_Files
    "AudioEngine/AudioEngine.h"
    "Camera2.5D/CameraManager.h"
    "Camera2.5D/ICamera.h"
    "Camera2.5D/PerspectiveCamera.h"
    "Renderers/LineRenderer/LineRenderer.h"
    "GECS/Animators/Animation.h"
    "GECS/Animators/AnimatorComponent.h"
    "GECS/Animators/AnimatorManager.h"
    "GECS/Animators/FlashAnimation.h"
    "GECS/Animators/FlashAnimatorComponent.h"
    "GECS/Components/Util/ButtonComponent.h"
    "GECS/Components/Basic/ColliderComponent.h"
    "GECS/Components.h"
    "GECS/GECS.h"
    "GECS/GECSManager.h"
    "GECS/Components/Util/GridComponent.h"
    "GECS/Components/Basic/Rectangle_w_Color.h"
    "GECS/Components/Util/RigidBodyComponent.h"
    "GECS/Components/Basic/SpriteComponent.h"
    "GECS/Components/Basic/TransformComponent.h"
    "GLSLProgram.h"
    "GLTexture.h"
    "GraphScreen/IScene.h"
    "GraphScreen/AppInterface.h"
    "GraphScreen/SceneList.h"
    "Grid/Grid.h"
    "ImGuiInterface/ImGuiInterface.h"
    "InputManager/InputManager.h"
    "picoPNG.h"
    "PNG_Letters.h"
    "ResourceManager/ResourceManager.h"
    "Renderers/FrameBuffer/Framebuffer.h"
    "Renderers/PlaneModelRenderer/PlaneModelRenderer.h"
    "TextureManager/TextureManager.h"
    "BaseFPSLimiter/BaseFPSLimiter.h"
    "Vertex.h"
    "Window/Window.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "AudioEngine/AudioEngine.cpp"
    "Camera2.5D/OrthoCamera.h"
    "Renderers/LineRenderer/LineRenderer.cpp"
    "GECS/GECS.cpp"
    "GraphScreen/IScene.cpp"
    "GraphScreen/AppInterface.cpp"
    "GraphScreen/ScreenIndices.h"
    "GraphScreen/SceneList.cpp"
    "Grid/Grid.cpp"
    "ImGuiInterface/ImGuiInterface.cpp"
    "imgui/imgui.cpp"
    "imgui/imgui_combo_autoselect.cpp"
    "imgui/imgui_demo.cpp"
    "imgui/imgui_draw.cpp"
    "imgui/imgui_impl_opengl3.cpp"
    "imgui/imgui_impl_sdl2.cpp"
    "imgui/imgui_tables.cpp"
    "imgui/imgui_widgets.cpp"
    "imgui/implot.cpp"
    "imgui/implot_items.cpp"
    "InputManager/InputManager.cpp"
    "PNG_Letters.cpp"
    "ResourceManager/ResourceManager.cpp"
    "Renderers/FrameBuffer/Framebuffer.cpp"
    "Renderers/PlaneModelRenderer/PlaneModelRenderer.cpp"
    "TextureManager/TextureManager.cpp"
    "BaseFPSLimiter/BaseFPSLimiter.cpp"
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
#include_directories(${SDL2_MIXER_INCLUDE_DIRS})

if(SDL2_mixer_FOUND)
    message(STATUS "Found SDL2_mixer: ${SDL2_MIXER_INCLUDE_DIRS}")
else()
    message(STATUS "Checking in: ${CMAKE_PREFIX_PATH}")
    message(WARNING "SDL2_mixer not found!")
endif()

#Find OpenGL library
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})


# Find Glew library
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS}) # LIBS added at end

#set(GLEW_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/deps/include/GL")
#include_directories(${GLEW_INCLUDE_DIR})

# Set the local include of IMGUI
set(IMGUI_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/deps/include/imgui")
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
        ">"
    )
endif()

message(" additional lib dependencies: ${ADDITIONAL_LIBRARY_DEPENDENCIES}")
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES})

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_mixer::SDL2_mixer)
message(STATUS "Linking SDL2_mixer with ${PROJECT_NAME} using ${SDL2_MIXER_LIBRARIES}")
#target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main)

target_link_libraries(${PROJECT_NAME} PRIVATE ${GLEW_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_LIBRARIES})
