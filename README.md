# TazGraph (40.9 MB)

![Graph Scene Screenshot](TazGraph/assets/Sprites/readme-1.png)

TazGraph is a modular, high-performance 3D graph editor built in C++ with OpenGL, designed for efficient visualization and interaction with large-scale graph data.


## Getting Started
create a build folder in root
necessary programs: cmake, g++
Ubuntu: 
`sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev libglew-dev libglm-dev libopengl`

`sudo apt install python3-dev libpython3-dev python3.12-dev libjs-sphinxdoc`

if libopengl does not exist:
`sudo apt install freeglut3-dev mesa-common-dev`

><b>Windows (Visual Studio 2022)</b>\
The project was primarily tested on Visual Studio 2022. 
>- Build settings are defined in the .vcxproj file.
>- Some large libraries (e.g., SDL, GLEW, Python) are not included locally in the repository. Instead, only their DLLs are committed. From these DLLs you can check which versions were used, but you may also use your own versions.
>- Library paths can be configured either through Project Properties in Visual Studio or directly by editing the .vcxproj file to point to the locations of the libraries on your system. (e.g. `$(SolutionDir)deps/lib;C:\Users\lefte\AppData\Local\Programs\Python\Python313\libs;`, `$(SolutionDir)deps\include;C:\Users\lefte\AppData\Local\Programs\Python\Python313\include`)

Start by cloning the repository with
Github:
`git clone --recursive https://github.com/mujinGr/TazGraph.git`
GitLab:
`git clone --recursive https://carvgit.ics.forth.gr/kotsonas/tazgraph.git`

If the repository was cloned non-recursively previously, use `git submodule update --init` to clone the necessary submodules.

### Build 
`mkdir build`
\
`cd build`
\
`cmake -DCMAKE_BUILD_TYPE=Release ..`
\
`make`

in TazGraph/TazGraph/:
`../build/TazGraph/TazGraph `**`N`**
where N is the number of threads to use

**For Windows**:\
In root folder:
`msbuild TazGraph.sln /p:Configuration=Release /p:Platform=x64`

In TazGraph/:
`../x64/Release/TazGraph.exe `**`N`**
where N is the number of threads to use

**Arguments**:\
[--num-threads=X] ${\color{green} \# \space number \space of \space worker\space threads\space (int)}$ \
[--MSAA=V] ${\color{green} \# \space multisample\space anti-aliasing\space level\space (int, e.g. 0, 2, 4, 8)}$\
[--open-file=Y] ${\color{green} \# \space path\space to\space file\space opened\space on\space startup\space (string)}$\
[--initial-timestamp=Z] ${\color{green} \#\space initial\space time\space value\space when\space starting\space playback/simulation\space (float\space or\space int)}$\
[--initial-step=W] ${\color{green} \#\space initial\space simulation\space step/frame index (int)}$\
[--use-python=C] ${\color{green} \#\space enable\space Python\space scripting\space (0\space =\space off,\space 1\space =\space on)}$\
[--bg-color=FR,FG,FB,FA] ${\color{green} \#\space background\space color\space as\space RGBA\space floats\space in\space range\space [0,1]}$\
[--grid=G] ${\color{green} \#\space grid\space mode\space or\space grid\space size\space (int\space or\space enum,\space depending\space on\space implementation)}$

## Intro

Originally developed to support HPC simulation workflows, TazGraph enables real-time rendering of over 10,000 nodes and 60,000 links on consumer-grade hardware. It provides a flexible and extensible interface for visualizing complex systems using intuitive node-link diagrams.

TazGraph is powered by a custom Entity-Component System (ECS) inspired by game engines. Entities (nodes, links, or empty) are modular, scriptable, and dynamically composed at runtime. The system supports multi-threaded batching, instanced rendering, and a 3D interactive workspace with support for multiple graphs and statistical overlays.

Unlike many existing graph tools that are either limited in scope or hard to extend, TazGraph offers a lightweight, cross-platform platform ideal for rapid prototyping, HPC monitoring, and deep structural analysis.

#### Built with:
- C++ & OpenGL
- ImGui & ImPlot
- Multithreaded ECS
- Custom renderer batching pipeline

Built with performance and flexibility in mind, TazGraph integrates 
real-time rendering, multithreading, and intuitive editing features 
into a unified environment.

#### Features:
- 3D World  
- Custom Renderers  
- Multithreading  
- Path Links Integration  
- Multiple Editor  Tabs  
- 3D Models with Lighting  
- Simulation Play  

## TODO

- [ ] Have beautiful and convinient UI 
- [ ] Extend Python API for nodes and on update execute
- [ ] Take Apache Arrow Data and execute large simulations

![alt text](TazGraph/assets/Sprites/Screenshot_9.png)
![alt text](TazGraph/assets/Sprites/Screenshot_1.png) 
![alt text](TazGraph/assets/Sprites/Screenshot_2.png)

## Developer Guide: [DoxyDevGuide](https://mujingr.github.io/TazGraph/html/dev_guide.html)

Full Documentation:
[DoxyDocument](https://mujingr.github.io/TazGraph/html/index.html)
\
Or download the pdf documentation: [PDF Documentation](https://github.com/mujinGr/TazGraph/blob/docs/docs/latex/refman.pdf)



[![Buy Me a Coffee](https://img.shields.io/badge/Buy%20Me%20a%20Coffee-support%20me-FFDD00?style=for-the-badge&logo=buy-me-a-coffee)](https://www.buymeacoffee.com/mujingr)
