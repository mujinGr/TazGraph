set(PROJECT_NAME TazGraph)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Recursively find all header and source files in Components and all subdirectories
file(GLOB_RECURSE EDITOR_COMPONENTS_HEADERS "Src/EditorIMGUI/Components/*.h")
file(GLOB_RECURSE EDITOR_COMPONENTS_SOURCES "Src/EditorIMGUI/Components/*.cpp")

################################################################################
# Source groups
################################################################################
# Grab all .h and .cpp files recursively from Src/
file(GLOB_RECURSE Header_Files "Src/*.h")
file(GLOB_RECURSE Source_Files "Src/*.cpp")

# Organize headers in IDE (Visual Studio, Xcode, etc.)
source_group("Header Files" FILES ${Header_Files})
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
)

add_compile_options(-pedantic)

################################################################################
# Setting libs with find_package
################################################################################
# Find SDL2 core library
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Find SDL2_image core library
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})

# Find SDL2 core library
find_package(SDL2_ttf REQUIRED)
include_directories(${SDL2_TTF_INCLUDE_DIRS})

find_package(Python3 REQUIRED COMPONENTS Development)
include_directories(${Python3_INCLUDE_DIRS})

# COPY FROM PROPERTIES IN MSVS
set(VENDORS_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/vendors")
include_directories(${VENDORS_INCLUDE_DIR})

# Set the local include of IMGUI
set(GLM_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/vendors/glm")
include_directories(${GLM_INCLUDE_DIR})

set(IMGUI_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/vendors/imgui")
include_directories(${IMGUI_INCLUDE_DIR})

set(IMGUI_BACKENDS_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/vendors/imgui/backends")
include_directories(${IMGUI_BACKENDS_INCLUDE_DIR})

set(IMPLOT_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/vendors/implot")
include_directories(${IMPLOT_INCLUDE_DIR})

set(IMGUIZMO_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/vendors/ImGuizmo")
include_directories(${IMGUIZMO_INCLUDE_DIR})

set(TINYXML_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/vendors/tinyxml2")
include_directories(${TINYXML_INCLUDE_DIR})

set(JSONPARSER_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/vendors/JSONParser_Mujin/include")
include_directories(${JSONPARSER_INCLUDE_DIR})

set(DOTPARSER_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/vendors/DotParser/DotParser")
include_directories(${DOTPARSER_INCLUDE_DIR})

set(PYBIND11_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/vendors/pybind11/include")
include_directories(${PYBIND11_INCLUDE_DIR})

set(DOTPARSER_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/vendors/simdump/include")
include_directories(${DOTPARSER_INCLUDE_DIR})
################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE TazGraph)

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
# MSVC runtime library
################################################################################
get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:Debug>:
            MultiThreadedDebugDLL
        >
        $<$<NOT:$<OR:$<CONFIG:Debug>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})

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
                /Gy;
                /O2;
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
                /Gy;
                /O2;
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
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE
        )
    endif()
endif()

################################################################################
# Dependencies
################################################################################
# Link with other targets.
set(TAZGRAPHENGINE_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/TazGraphEngine")
target_include_directories(${PROJECT_NAME} PRIVATE
    ${TAZGRAPHENGINE_INCLUDE_DIR}  
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    TazGraphEngine
)

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:Debug>:"
            "SDL2;"
            "SDL2main;"
            "SDL2_image;"
            "SDL2_ttf;"
            "SDL2_mixer;"
            "TazGraphEngine"
        ">"
    )
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_IMAGE_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_ttf::SDL2_ttf)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Python3_LIBRARIES})
