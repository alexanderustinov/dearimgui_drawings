cmake_minimum_required(VERSION 3.3)
project(dearimgui_drawings CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 CONFIG REQUIRED)
include(FindOpenGL)

include_directories(external/imgui/backends external/imgui ${OPENGL_INCLUDE_DIR})

add_executable(${PROJECT_NAME}
        src/main.cpp
        external/imgui/imgui.cpp
        external/imgui/imgui_widgets.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_demo.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
        external/imgui/backends/imgui_impl_sdl2.cpp
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        ${OPENGL_LIBRARIES}
)
