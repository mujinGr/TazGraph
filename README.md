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

><b>Windows (Visual Studio 2022)</b> <br>
The project was primarily tested on Visual Studio 2022. <br>
>- Build settings are defined in the .vcxproj file.<br>
>- Some large libraries (e.g., SDL, GLEW, Python) are not included locally in the repository. <br>
Instead, only their DLLs are committed. From these DLLs you can check which versions were used, <br>
but you may also use your own versions.<br>
>- Library paths can be configured either through Project Properties in Visual Studio or <br>
directly by editing the .vcxproj file to point to the locations of the libraries on your system.<br>
(e.g. `$(SolutionDir)deps/lib;C:\Users\lefte\AppData\Local\Programs\Python\Python313\libs;`, `$(SolutionDir)deps\include;C:\Users\lefte\AppData\Local\Programs\Python\Python313\include`)

Start by cloning the repository with <br>
\
Github:<br>
`git clone --recursive https://github.com/mujinGr/TazGraph.git`
\
GitLab:<br>
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

- `--num-threads=X` <span style="color:green"># number of worker threads (int)</span>
- `--MSAA=V` <span style="color:green"># multisample anti-aliasing level (int, e.g. 0, 2, 4, 8)</span>
- `--open-file=Y` <span style="color:green"># path to file opened on startup (string)</span>
- `--initial-timestamp=Z` <span style="color:green"># initial time value when starting playback/simulation (float or int)</span>
- `--initial-step=W` <span style="color:green"># initial simulation step/frame index (int)</span>
- `--use-python=C` <span style="color:green"># enable Python scripting (0 = off, 1 = on)</span>
- `--bg-color=FR,FG,FB,FA` <span style="color:green"># background color as RGBA floats in range [0,1]</span>
- `--grid=G` <span style="color:green"># grid mode or grid size (int or enum)</span>

## Intro

Originally developed to support HPC simulation workflows, <br>
TazGraph enables real-time rendering of over 10,000 nodes and 60,000 links on consumer-grade hardware.<br>
It provides a flexible and extensible interface for visualizing complex systems using intuitive node-link diagrams.<br>
<br>
TazGraph is powered by a custom Entity-Component System (ECS) inspired by game engines. <br>
Entities (nodes, links, or empty) are modular, scriptable, and dynamically composed at runtime. <br>
The system supports multi-threaded batching, instanced rendering, and a 3D interactive workspace with support for multiple graphs and statistical overlays.<br>
<br>
Unlike many existing graph tools that are either limited in scope or hard to extend,<br>
TazGraph offers a lightweight, cross-platform platform ideal for rapid prototyping, HPC monitoring, and deep structural analysis.

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

![alt text](TazGraph/assets/Sprites/Screenshot_9.png)
![alt text](TazGraph/assets/Sprites/Screenshot_1.png) 
![alt text](TazGraph/assets/Sprites/Screenshot_2.png)

## Developer Guide: [DoxyDevGuide](https://mujingr.github.io/TazGraph/html/dev_guide.html)

Full Documentation:
[DoxyDocument](https://mujingr.github.io/TazGraph/html/index.html)
\
Or download the pdf documentation: [PDF Documentation](https://github.com/mujinGr/TazGraph/blob/docs/docs/latex/refman.pdf)



[![Buy Me a Coffee](https://img.shields.io/badge/Buy%20Me%20a%20Coffee-support%20me-FFDD00?style=for-the-badge&logo=buy-me-a-coffee)](https://www.buymeacoffee.com/mujingr)
