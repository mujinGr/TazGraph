set(PROJECT_NAME TazGraph)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# AppImage output name
set(APPIMAGE_NAME "TazGraph-${PROJECT_VERSION}-x86_64.AppImage")

# Check if we're building for AppImage
option(BUILD_APPIMAGE "Build AppImage bundle" OFF)

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
add_compile_definitions(TRACY_ENABLE)
add_compile_definitions(TRACY_NO_INVARIANT_CHECK)
add_compile_definitions(TRACY_TIMER_FALLBACK)
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

set(FILEBROWSER_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/vendors/imgui-filebrowser")
include_directories(${FILEBROWSER_INCLUDE_DIR})
################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE TazGraph)


if(BUILD_APPIMAGE)
    set(CMAKE_INSTALL_RPATH "$ORIGIN/lib")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()


################################################################################
# Dependencies
################################################################################
# Link with other targets.

set(RENDER_ENGINE_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/RenderEngine")
target_include_directories(${PROJECT_NAME} PRIVATE
    ${RENDER_ENGINE_INCLUDE_DIR}  
)


set(TAZGRAPHENGINE_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/TazGraphEngine")
target_include_directories(${PROJECT_NAME} PRIVATE
    ${TAZGRAPHENGINE_INCLUDE_DIR}  
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    TazGraphEngine
)

target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_IMAGE_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_ttf::SDL2_ttf)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Python3_LIBRARIES})

# Install target
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
    
# Install data files if needed
if(EXISTS "${PROJECT_SOURCE_DIR}/TazGraph/assets")
    install(DIRECTORY "${PROJECT_SOURCE_DIR}/TazGraph/assets" 
        DESTINATION .
        PATTERN ".git*" EXCLUDE
        PATTERN "*~" EXCLUDE
    )
endif()


function(create_desktop_file NAME COMMENT)
        set(DESKTOP_CONTENT
            "[Desktop Entry]\\n"
            "Type=Application\\n"
            "Name=${NAME}\\n"
            "Comment=${COMMENT}\\n"
            "Exec=${NAME}\\n"
            "Icon=${NAME}\\n"
            "Categories=Graphics;Science;\\n"
            "Terminal=false\\n"
        )
endfunction()
    
# AppImage target
if(BUILD_APPIMAGE)
    # Check for appimagetool
    find_program(APPIMAGETOOL_EXE appimagetool-x86_64.AppImage
        HINTS ${CMAKE_BINARY_DIR}
    )
        
    if(NOT APPIMAGETOOL_EXE)
        message(WARNING "appimagetool not found. Downloading...")
        file(DOWNLOAD 
            https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage
            ${CMAKE_BINARY_DIR}/appimagetool-x86_64.AppImage
            TIMEOUT 60
        )
        file(CHMOD ${CMAKE_BINARY_DIR}/appimagetool-x86_64.AppImage FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE)
        set(APPIMAGETOOL_EXE ${CMAKE_BINARY_DIR}/appimagetool-x86_64.AppImage)
    endif()
        
    # need to first create them before copying them inside AppDir
    file(WRITE "${CMAKE_BINARY_DIR}/AppRun"
    "#!/bin/bash
    APPDIR=\"\${APPDIR:-\$(dirname \"\$(readlink -f \"\$0\")\")}\"\n
    export LD_LIBRARY_PATH=\"\$APPDIR/TazGraph:\$LD_LIBRARY_PATH\"
    export PATH=\"\$APPDIR/TazGraph:\$PATH\"

    cd \"\$APPDIR/TazGraph\"
    exec \"\$APPDIR/TazGraph/TazGraph\" \"\$@\"
    ")

    file(CHMOD ${CMAKE_BINARY_DIR}/AppRun FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        
    file(WRITE "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.desktop" ${DESKTOP_CONTENT})
    
    # Custom target for AppImage
    add_custom_target(appimage
        # Build the project
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config ${CMAKE_BUILD_TYPE}
        
        # Create AppDir structure with TazGraph and TazGraphEngine folders
        COMMAND rm -rf ${CMAKE_BINARY_DIR}/AppDir
        COMMAND mkdir -p ${CMAKE_BINARY_DIR}/AppDir/TazGraph/assets
        COMMAND mkdir -p ${CMAKE_BINARY_DIR}/AppDir/TazGraphEngine/src/GECS
        
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/AppRun ${CMAKE_BINARY_DIR}/AppDir/
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.desktop ${CMAKE_BINARY_DIR}/AppDir/


        # Copy executable to TazGraph folder as well
        COMMAND cp $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_BINARY_DIR}/AppDir/TazGraph/
        
        # Copy and icon
        COMMAND cp ${PROJECT_SOURCE_DIR}/TazGraph/assets/Sprites/TazGraph_whitebg-8.png ${CMAKE_BINARY_DIR}/AppDir/${PROJECT_NAME}.png
        
        # Copy assets to TazGraph/assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
                ${PROJECT_SOURCE_DIR}/TazGraph/assets 
                ${CMAKE_BINARY_DIR}/AppDir/TazGraph/assets
                
        # Copy shaders to TazGraph/assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
                ${PROJECT_SOURCE_DIR}/TazGraph/Src/Shaders 
                ${CMAKE_BINARY_DIR}/AppDir/TazGraph/Src/Shaders
                
        # Copy animators to TazGraphEngine/GECS
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
                ${PROJECT_SOURCE_DIR}/TazGraphEngine/src/GECS/Animators 
                ${CMAKE_BINARY_DIR}/AppDir/TazGraphEngine/src/GECS/Animators
                
        # Copy Sounds to TazGraph/Sounds
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
                ${PROJECT_SOURCE_DIR}/TazGraph/Sounds 
                ${CMAKE_BINARY_DIR}/AppDir/TazGraph/Sounds
    

        COMMAND ${APPIMAGETOOL_EXE} ${CMAKE_BINARY_DIR}/AppDir 
                ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.AppImage

        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Creating AppImage with TazGraph and TazGraphEngine structure"
        VERBATIM
    )

endif()  # BUILD_APPIMAGE

