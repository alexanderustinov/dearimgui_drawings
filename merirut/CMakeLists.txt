cmake_minimum_required(VERSION 3.3)
project(dearimgui_drawings CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 CONFIG REQUIRED)
include(FindOpenGL)

include_directories(dependencies/imgui/backends dependencies/imgui ${OPENGL_INCLUDE_DIR})

add_executable(${PROJECT_NAME}
        src/main.cpp
        dependencies/imgui/imgui.cpp
        dependencies/imgui/imgui_widgets.cpp
        dependencies/imgui/imgui_tables.cpp
        dependencies/imgui/imgui_draw.cpp
        dependencies/imgui/imgui_demo.cpp
        dependencies/imgui/backends/imgui_impl_opengl3.cpp
        dependencies/imgui/backends/imgui_impl_sdl2.cpp
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        ${OPENGL_LIBRARIES}
)
