Code Documentation: `https://mujingr.github.io/TazGraph/html/index.html` 

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
`msbuild TazGraph.sln /p:Configuration=Debug /p:Platform=x64`

In TazGraph/:
`../x64/Debug/TazGraph.exe`
