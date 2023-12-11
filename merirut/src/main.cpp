#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"
#include "imgui.h"
#include <SDL.h>
#include <cmath>
#include <string>
#include <cstdio>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

void apply_rules(std::string &result, char ch) {
    if (ch == 'A') {
        result += "A-B--B+A++AA+B-";
    } else if (ch == 'B') {
        result += "+A-BB--B-A++A+B";
    } else {
        result.push_back(ch);
    }
}

std::string generate_gosper(const std::string &axiom, int iterations) {
    std::string result = axiom;
    for (int i = 0; i < iterations; ++i) {
        std::string temp;
        for (char ch : result) {
            apply_rules(temp, ch);
        }
        result = temp;
    }
    return result;
}

void draw_gosper(ImDrawList *drawlist, const std::string &instructions, float length, float angle, ImVec2 start, ImVec4 &color) {
    ImVec2 pos = start;
    float direction = 90;
    std::vector<std::tuple<ImVec2, ImVec2, ImVec4>> lines;

    for (char command : instructions) {
        if (command == 'A' || command == 'B') {
            ImVec2 newPos = ImVec2(pos.x + length * cosf(direction * M_PI / 180.0f),
                                   pos.y + length * sinf(direction * M_PI / 180.0f));
            lines.emplace_back(pos, newPos, color);
            pos = newPos;
        } else if (command == '+') {
            direction -= angle;
        } else if (command == '-') {
            direction += angle;
        }
    }

    for (const auto &line : lines) {
        drawlist->AddLine(std::get<0>(line), std::get<1>(line), IM_COL32(255, 255, 255, 255));
    }
}

int main(int, char **) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

#if defined(IMGUI_IMPL_OPENGL_ES2)
    const char *glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    const char *glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    const char *glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags =
            (SDL_WindowFlags) (SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window *window = SDL_CreateWindow(
            "Gosper Curve", SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(0); // Disable vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    bool done = false;

    std::string axiom = "A";
    int iterations = 4;
    float length = 10.0f;
    float angle = 60.0f;
    ImVec2 start = ImVec2(650.0f, 450.0f);

    std::string instructions = generate_gosper(axiom, iterations);

    ImVec4 lineColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGuiViewport *viewport = ImGui::GetMainViewport();
        auto size = viewport->WorkSize;
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(size);

        ImGui::Begin("Gosper Curve", nullptr, ImGuiWindowFlags_NoDecoration);

        const ImVec2 center{size.x / 2, size.y / 2};
        const ImVec2 other{
                center.x + static_cast<float>(std::cos(angle) * length),
                center.y + static_cast<float>(std::sin(angle) * length)};
        angle = angle + 0.5f * 10 * M_PI / 180;

        auto drawlist = ImGui::GetWindowDrawList();
        draw_gosper(drawlist, instructions, length, angle, start, lineColor);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

        ImGui::End();

        ImGui::Render();
        glViewport(0, 0, (int) io.DisplaySize.x, (int) io.DisplaySize.y);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}