# ui_dig
Сделано на windows из https://github.com/ocornut/imgui/blob/master/examples/example_sdl2_opengl3/main.cpp
```shell
# добавить C:\msys64\mingw64\bin в PATH
# установить mingw-w64-x86_64-ninja mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-SDL2 в MSYS2
# либо build-essential libsdl2-dev в debian(-based)
git clone --recursive https://github.com/alexanderustinov/ui_dig.git
mkdir build ; cd build
cmake ../
ninja
```