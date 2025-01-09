create a build folder in root
necessary programs: cmake, g++
WSL: 
sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev libglew-dev libglm-dev libopengl
if libopengl does not exist:
sudo apt install freeglut3-dev mesa-common-dev

In build/:
cmake ..
make

in TazGraph/TazGraph/:
../build/TazGraph/TazGraph

For Windows:
In root folder:
msbuild TazGraph.sln /p:Configuration=Debug /p:Platform=x64

In TazGraph/:
 ../x64/Debug/TazGraph.exe
