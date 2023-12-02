cmake_minimum_required(VERSION 3.11)
project(ui_dig CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

include_directories( include external/imgui ${OPENGL_INCLUDE_DIR} ${SDL2_INCLUDE_DIR} )

add_executable(ui_dig
    src/main.cpp
    external/imgui/imgui.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
    external/imgui/backends/imgui_impl_sdl2.cpp
    )

target_link_libraries(ui_dig ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES} SDL2::SDL2 SDL2::SDL2main)
