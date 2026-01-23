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

# Install shaders from Src/Shaders
if(EXISTS "${PROJECT_SOURCE_DIR}/TazGraph/Src/Shaders")
    install(DIRECTORY "${PROJECT_SOURCE_DIR}/TazGraph/Src/Shaders"
        DESTINATION assets
        FILES_MATCHING PATTERN "*.vert" PATTERN "*.frag" PATTERN "*.glsl"
        PATTERN ".git*" EXCLUDE
        PATTERN "*~" EXCLUDE
    )
endif()

# Install animators from TazGraphEngine
if(EXISTS "${PROJECT_SOURCE_DIR}/TazGraphEngine/src/GECS/Animators")
    install(DIRECTORY "${TAZGRAPHENGINE_DIR}/src/GECS/Animators"
        DESTINATION assets/GECS
        PATTERN ".git*" EXCLUDE
        PATTERN "*~" EXCLUDE
    )
endif()

# Install sounds if exists
if(EXISTS "${PROJECT_SOURCE_DIR}/TazGraph/Sounds")
    install(DIRECTORY "${PROJECT_SOURCE_DIR}/Sounds"
        DESTINATION .
        PATTERN ".git*" EXCLUDE
        PATTERN "*~" EXCLUDE
    )
endif()
    
# Install desktop file  
if(EXISTS "${PROJECT_SOURCE_DIR}/deploy/${PROJECT_NAME}.desktop")
    install(FILES "${PROJECT_SOURCE_DIR}/deploy/${PROJECT_NAME}.desktop" DESTINATION share/applications)
endif()
    
# Install icons
if(EXISTS "${PROJECT_SOURCE_DIR}/TazGraph/assets/Sprites")
    install(DIRECTORY "${PROJECT_SOURCE_DIR}/deploy/icons" DESTINATION share/icons/hicolor)
endif()
    
# AppImage target
if(BUILD_APPIMAGE)
    # Check for linuxdeploy
    find_program(LINUXDEPLOY_EXE linuxdeploy-x86_64.AppImage
        HINTS /usr/local/bin /usr/bin /bin
        PATHS ${CMAKE_SOURCE_DIR}
    )
        
    if(NOT LINUXDEPLOY_EXE)
        message(WARNING "linuxdeploy not found. Downloading...")
        file(DOWNLOAD 
            https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
            ${CMAKE_BINARY_DIR}/linuxdeploy-x86_64.AppImage
            TIMEOUT 60
        )
        file(CHMOD ${CMAKE_BINARY_DIR}/linuxdeploy-x86_64.AppImage FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE)
        set(LINUXDEPLOY_EXE ${CMAKE_BINARY_DIR}/linuxdeploy-x86_64.AppImage)
    endif()
        
    # Check for appimagetool
    find_program(APPIMAGETOOL_EXE appimagetool-x86_64.AppImage
        HINTS /usr/local/bin /usr/bin /bin
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
        
    # Create desktop file if it doesn't exist
    if(NOT EXISTS "${PROJECT_SOURCE_DIR}/deploy/${PROJECT_NAME}.desktop")
        file(WRITE "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.desktop"
            "[Desktop Entry]
            Type=Application
            Name=TazGraph
            Comment=Graph Visualization Tool
            Exec=${PROJECT_NAME}
            Icon=${PROJECT_NAME}
            Categories=Graphics;Science;
            Terminal=false
            ")
    endif()
        
    # Create icon if it doesn't exist
    if(NOT EXISTS "${PROJECT_SOURCE_DIR}/deploy/icons/256x256/apps/${PROJECT_NAME}.png")
        # Create a simple placeholder icon
        find_program(CONVERT_EXE convert)
        if(CONVERT_EXE)
            execute_process(
                COMMAND ${CONVERT_EXE} -size 256x256 xc:#4A90E2 -pointsize 72 -fill white -gravity center -annotate 0 "TG" ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.png
            )
        endif()
    endif()
        
    # Custom target for AppImage
    add_custom_target(appimage
        # Build the project
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config ${CMAKE_BUILD_TYPE}
        
        # Create AppDir structure
        COMMAND rm -rf ${CMAKE_BINARY_DIR}/AppDir
        COMMAND mkdir -p ${CMAKE_BINARY_DIR}/AppDir/usr/bin
        COMMAND mkdir -p ${CMAKE_BINARY_DIR}/AppDir/assets
        
        # Copy executable
        COMMAND cp $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_BINARY_DIR}/AppDir/usr/bin/
        
        # Copy AppRun script
        COMMAND cp ${CMAKE_BINARY_DIR}/AppRun ${CMAKE_BINARY_DIR}/AppDir/
        COMMAND chmod +x ${CMAKE_BINARY_DIR}/AppDir/AppRun
        
        # Copy desktop file and icon
        COMMAND cp ${DESKTOP_FILE} ${CMAKE_BINARY_DIR}/AppDir/
        COMMAND cp ${ICON_FILE} ${CMAKE_BINARY_DIR}/AppDir/${PROJECT_NAME}.png
        
        # Copy assets from TazGraph/assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
                ${PROJECT_SOURCE_DIR}/assets 
                ${CMAKE_BINARY_DIR}/AppDir/assets
                
        # Copy shaders
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
                ${PROJECT_SOURCE_DIR}/Src/Shaders 
                ${CMAKE_BINARY_DIR}/AppDir/assets/Shaders
                
        # Copy animators from TazGraphEngine
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
                ${TAZGRAPHENGINE_DIR}/src/GECS/Animators 
                ${CMAKE_BINARY_DIR}/AppDir/assets/GECS/Animators
                
        # Copy sounds if they exist
        COMMAND if [ -d "${PROJECT_SOURCE_DIR}/Sounds" ]; then 
                ${CMAKE_COMMAND} -E copy_directory 
                ${PROJECT_SOURCE_DIR}/Sounds 
                ${CMAKE_BINARY_DIR}/AppDir/Sounds; fi
                
        # Run linuxdeploy to bundle dependencies
        COMMAND cd ${CMAKE_BINARY_DIR} && 
                ${LINUXDEPLOY} 
                --appdir=AppDir 
                --executable=AppDir/usr/bin/${PROJECT_NAME}
                --desktop-file=AppDir/${PROJECT_NAME}.desktop
                --icon-file=AppDir/${PROJECT_NAME}.png
                --output=appimage
        
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Creating AppImage with all assets"
        VERBATIM
    )
        
    # Alternative simpler AppImage target
    add_custom_target(appimage-simple
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/AppDir/usr/bin
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_BINARY_DIR}/AppDir/usr/bin/
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.desktop ${CMAKE_BINARY_DIR}/AppDir/
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.png ${CMAKE_BINARY_DIR}/AppDir/
        COMMAND ${LINUXDEPLOY_EXE} --appdir ${CMAKE_BINARY_DIR}/AppDir --output appimage
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS ${PROJECT_NAME}
        COMMENT "Creating simple AppImage"
    )
        
    # Python plugin for linuxdeploy (if using Python)
    add_custom_target(appimage-with-python
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target install --config ${CMAKE_BUILD_TYPE}
        COMMAND rm -rf ${CMAKE_BINARY_DIR}/AppDir
        # Download Python plugin
        COMMAND wget -q -O ${CMAKE_BINARY_DIR}/linuxdeploy-plugin-python-x86_64.AppImage 
            https://github.com/linuxdeploy/linuxdeploy-plugin-python/releases/download/continuous/linuxdeploy-plugin-python-x86_64.AppImage
        COMMAND chmod +x ${CMAKE_BINARY_DIR}/linuxdeploy-plugin-python-x86_64.AppImage
        # Run linuxdeploy with Python plugin
        COMMAND ${LINUXDEPLOY_EXE} 
            --appdir=${CMAKE_BINARY_DIR}/AppDir
            --executable=${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}
            --desktop-file=${CMAKE_BINARY_DIR}/${PROJECT_NAME}.desktop
            --icon-file=${CMAKE_BINARY_DIR}/${PROJECT_NAME}.png
            --plugin=python
            --output=appimage
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS ${PROJECT_NAME}
        COMMENT "Creating AppImage with Python support"
    )
        
endif() # BUILD_APPIMAGE

