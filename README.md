# SimpleAStar

A very simple and naive implementation of A* Search algorithm, it is not optimized.

## Building
First install all dependencies needed to build raylib from source, then fetch code with submodules:
```bash
git clone --recurse-submodules https://github.com/cedmundo/SimpleAStar.git
```
Build with CMake:
```bash
mkdir -p cmake-build-debug
cmake -DCMAKE_BUILD_TYPE=debug -B cmake-build-debug
cmake --build cmake-build-debug 
```
Done.