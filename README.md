# TazGraph (40.9 MB)

![Graph Scene Screenshot](TazGraph/assets/Sprites/readme-1.png)

TazGraph is a 3D graph editor and visualization tool built using C++, OpenGL, ImGui and SDL2. It provides interactive manipulation of nodes and edges with customizable rendering, ECS-based architecture, and multithreaded performance optimizations.

[DoxyDocument](https://mujingr.github.io/TazGraph/html/index.html)

## Build Instructions
create a build folder in root
necessary programs: cmake, g++
WSL: 
`sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev libglew-dev libglm-dev libopengl`
if libopengl does not exist:
`sudo apt install freeglut3-dev mesa-common-dev`

Start by cloning the repository with
`git clone --recursive https://carvgit.ics.forth.gr/kotsonas/tazgraph`

In build/:
`cmake -DCMAKE_BUILD_TYPE=Release ..`
`make`

in TazGraph/TazGraph/:
`../build/TazGraph/TazGraph`

For Windows:
In root folder:
`msbuild TazGraph.sln /p:Configuration=Release /p:Platform=x64`

In TazGraph/:
`../x64/Debug/TazGraph.exe`

## Developer Integration Guide

### Core Architecture:

TazGraph uses a **custom ECS** where entities (such as nodes, links, and emptyEntities) are composed of components. Components are plain data-only structures, like `TransformComponent` and `PollingComponent`.

The ECS is managed by **`GECSManager`**, which handles creation, removal, update, and drawing of entities. Entity processing is based on **groups** (`EntityGroups`) for rendering and update.

### Scene System

There are two scenes implemented:

- `MainMenuScene`
- `Graph`

Each scene controls its own logic, similar to a game state system. In the `Graph` Scene:
- You can load **multiple graphs** in tabs, each saved in a **Manager**.
- Developers **shouldn’t need to modify the core of graph engine logic** (TazGraphEngine/GECS/Core) — instead, use the main scene's update function (TazGraph/Src/Graph.cpp) to hook into behaviors like:
  - Grouping nodes
  - Flash-frame animations
  - UI interactions

### Graph Rendering Pipeline

All rendering logic is handled in `Graph::draw()`.

#### 1. Shader Setup

GLSL shader programs are loaded and set up using the resource manager. Uniforms (e.g. camera matrix, rotation) are passed before rendering:

```cpp
_resourceManager.setupShader(glsl_color, *main_camera2D);
GLint pLocation = glsl_color.getUniformLocation("rotationMatrix");
glUniformMatrix4fv(pLocation, 1, GL_FALSE, glm::value_ptr(rotationMatrix));
```

#### 2. Render Pass Initialization
Before batching begins, the renderers are prepared. This includes setting up internal buffers and clearing previous frame data.

```cpp
	_PlaneModelRenderer.initTextureQuadBatch(
		manager->getVisibleGroup<NodeEntity>(Manager::groupRenderSprites).size()
	);
...

	_PlaneModelRenderer.initBatchSize();
...
```

#### 3. Render Batching (Multithreaded)

Entities are grouped and passed to the appropriate renderer via renderBatch. This uses multithreading to fill buffers (meshElements & meshArrays) efficiently.

``` cpp
renderBatch(
    manager->getVisibleGroup<LinkEntity>(Manager::groupLinks_0),
    _LineRenderer
);
```

#### 4. Instance Rendering

After batching, each renderer draws its instances and clears its buffers:

``` cpp
_PlaneColorRenderer.end();
_PlaneColorRenderer.renderBatch(&glsl_color);
glsl_color.unuse();
```
Repeat for each renderer (e.g. _LineRenderer, _PlaneModelRenderer, _LightRenderer) in your preferred draw order.

<code style="color : limegreen">Not all Entity Components can draw with all renderers (e.g. Sprite Component is used to specifically draw for PlaneModelRenderer). 
The draw function in the component indicates the renderers used: `void draw(size_t v_index, `<code style="color : red">PlaneModelRenderer</code>`&  batch, TazGraphEngine::Window& window)
	{
	...
	}
`</code>

<!-- [Adding Graph Loaders](#adding-graph-loaders)

[Adding Custom Renderers](#adding-custom-renderers) -->

### Custom Renderers

Renderers in TazGraph are grouped by the attributes they support, which directly affects the shaders they use. This design makes rendering highly memory-efficient, but each renderer is tailored for a specific use case.

Each renderer sets a fixed number of per-vertex and per-instance attributes. For example, the `PlaneColorRenderer` expects shaders with the following layout:

```
layout (location = 0) in vec3 aPos; //vertex Pos
layout (location = 1) in vec3 aNormal; //vertex Normal

layout (location = 2) in vec3 instanceSize;
layout (location = 3) in vec3 instanceBodyCenter;
layout (location = 4) in vec3 instanceRotation;
layout (location = 5) in vec4 instanceColor;
```
Your GLSL shader must match the expected attribute layout of the renderer you're using.

Currently, the engine includes the following specific renderers:
- `PlaneColorRenderer` – for flat-colored, instanced 3D planes
- `PlaneModelRenderer` – for textured plane instances
- `LightRenderer` – for planes with lighting and shading
- `LineRenderer` – for simple colored lines (typically used for links)

To create a new renderer - for example, a **textured plane with shading** - you need to implement a new renderer class that matches the data layout and rendering logic required for that effect.

In the following folder: `TazGraphEngine/Renderers/PlaneRenderers/PlaneLitTextRenderer` use one of the existing renderers as a base, then write glsl shader to match the renderer under `TazGraph/Src/Shaders`, implement a `draw` function in a component that uses that renderer and then pass that renderer as argument for `renderBatch` in `Graph::draw()`.

### Graph Loaders (WIP)

Currently TazGraph has the ability to process txt and python files with a specific format. Developers are able to implement their own processing of files, by creating more functions inside `TazGraph/Src/Map`


[![Buy Me a Coffee](https://img.shields.io/badge/Buy%20Me%20a%20Coffee-support%20me-FFDD00?style=for-the-badge&logo=buy-me-a-coffee)](https://www.buymeacoffee.com/mujingr)
