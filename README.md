# TazGraph (40.9 MB)

![Graph Scene Screenshot](TazGraph/assets/Sprites/readme-1.png)

TazGraph is a modular, high-performance 3D graph editor built in C++ with OpenGL, designed for efficient visualization and interaction with large-scale graph data.


## Getting Started
create a build folder in root
necessary programs: cmake, g++
WSL: 
`sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev libglew-dev libglm-dev libopengl`
if libopengl does not exist:
`sudo apt install freeglut3-dev mesa-common-dev`

Start by cloning the repository with
`git clone --recursive https://carvgit.ics.forth.gr/kotsonas/tazgraph`

If the repository was cloned non-recursively previously, use `git submodule update --init` to clone the necessary submodules.

In build/:
\
`cmake -DCMAKE_BUILD_TYPE=Release ..`
\
`make`

in TazGraph/TazGraph/:
`../build/TazGraph/TazGraph`

For Windows:
In root folder:
`msbuild TazGraph.sln /p:Configuration=Release /p:Platform=x64`

In TazGraph/:
`../x64/Debug/TazGraph.exe`

## Intro

Originally developed to support HPC simulation workflows, TazGraph enables real-time rendering of over 10,000 nodes and 60,000 links on consumer-grade hardware. It provides a flexible and extensible interface for visualizing complex systems using intuitive node-link diagrams.

TazGraph is powered by a custom Entity-Component System (ECS) inspired by game engines. Entities (nodes, links, or empty) are modular, scriptable, and dynamically composed at runtime. The system supports multi-threaded batching, instanced rendering, and a 3D interactive workspace with support for multiple graphs and statistical overlays.

Unlike many existing graph tools that are either limited in scope or hard to extend, TazGraph offers a lightweight, cross-platform platform ideal for rapid prototyping, HPC monitoring, and deep structural analysis.

#### Built with:
- C++ & OpenGL
- ImGui & ImPlot
- Multithreaded ECS
- Custom renderer batching pipeline


## Developer Guide: [DoxyDevGuide](https://mujingr.github.io/TazGraph/html/dev_guide.html)

Full Documentation:
[DoxyDocument](https://mujingr.github.io/TazGraph/html/index.html)
\
Or download the pdf documentation: [PDF Documentation](https://github.com/mujinGr/TazGraph/blob/docs/docs/latex/refman.pdf)



[![Buy Me a Coffee](https://img.shields.io/badge/Buy%20Me%20a%20Coffee-support%20me-FFDD00?style=for-the-badge&logo=buy-me-a-coffee)](https://www.buymeacoffee.com/mujingr)
