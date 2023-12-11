cmake_minimum_required(VERSION 3.11)
project(Mikh_Ilya CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

include_directories( include imgui ${OPENGL_INCLUDE_DIR} ${SDL2_INCLUDE_DIR} )

add_executable(Mikh_Ilya
    src/main.cpp
    imgui/imgui.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_demo.cpp
    imgui/backends/imgui_impl_opengl3.cpp
    imgui/backends/imgui_impl_sdl2.cpp
    )

target_link_libraries(Mikh_Ilya ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES} )