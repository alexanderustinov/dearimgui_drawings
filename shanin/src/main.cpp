#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"
#include "imgui.h"
#include <SDL.h>
#include <cmath>
#include <math.h>
#include <stdio.h>
#include <vector>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

const double golden_ratio = (1 + std::sqrt(5.0)) / 2;
const double r1 = std::pow(1 / golden_ratio, 1 / golden_ratio);
const double r2 = std::pow(r1, 2);
const double angle1 = std::acos((1 + std::pow(r1, 2) - std::pow(r1, 4)) / (2 * r1));
const double angle2 = std::acos((1 + std::pow(r1, 4) - std::pow(r1, 2)) / (2 * std::pow(r1, 2)));

void golden_dragon(ImDrawList* drawlist, double x1, double y1, double x2, double y2, bool turn, int n) {
    if (n == 1) {
        drawlist->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), IM_COL32(255, 255, 255, 255), 1.0f);
    } else {
        double dist = std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));

        if (dist < 1) {
            drawlist->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), IM_COL32(255, 255, 255, 255), 1.0f);
        } else {
            double angle = std::atan2(y2 - y1, x2 - x1);

            double px, py;
            if (turn) {
                px = x1 + dist * r1 * std::cos(angle + angle1);
                py = y1 + dist * r1 * std::sin(angle + angle1);
            } else {
                px = x1 + dist * r2 * std::cos(angle - angle2);
                py = y1 + dist * r2 * std::sin(angle - angle2);
            }

            golden_dragon(drawlist, x1, y1, px, py, true, n - 1);
            golden_dragon(drawlist, px, py, x2, y2, false, n - 1);
        }
    }
}

int main(int, char **) {
  // Setup SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) !=
      0) {
    printf("Error: %s\n", SDL_GetError());
    return -1;
  }

  // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
  // GL ES 2.0 + GLSL 100
  const char *glsl_version = "#version 100";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
  // GL 3.2 Core + GLSL 150
  const char *glsl_version = "#version 150";
  SDL_GL_SetAttribute(
      SDL_GL_CONTEXT_FLAGS,
      SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 130";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

#ifdef SDL_HINT_IME_SHOW_UI
  SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_WindowFlags window_flags =
      (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
                        SDL_WINDOW_ALLOW_HIGHDPI);
  SDL_Window *window = SDL_CreateWindow(
      "Dear ImGui SDL2+OpenGL3 example", SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  SDL_GL_MakeCurrent(window, gl_context);
  SDL_GL_SetSwapInterval(0);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad;

  ImGui::StyleColorsDark();

  ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init(glsl_version);

  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  bool done = false;
    static float angle = 0;
    const int length = 100;
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

        {
            static float speed = 0.5f;
            static int counter = 0;

            ImGuiViewport *viewport = ImGui::GetMainViewport();
            auto size = viewport->WorkSize;
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(size);

            ImGui::Begin(
                "Hello, world!", NULL,
                ImGuiWindowFlags_NoDecoration);

            const ImVec2 center{size.x / 2, size.y / 2};

            int iterations = 15;
            std::vector<ImVec2> dragonPoints;
            golden_dragon(ImGui::GetWindowDrawList(), center.x, center.y - length, center.x, center.y + length, true, iterations);

            const float scale = 50.0f;
            for (const auto& point : dragonPoints) {
                ImVec2 scaledPoint = ImVec2(center.x + point.x * scale, center.y + point.y * scale);
                ImGui::GetWindowDrawList()->AddCircleFilled(scaledPoint, 2.0f, IM_COL32(255, 255, 255, 255));
            }

            counter++;
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                        1000.0f / io.Framerate, io.Framerate);

            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                    clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
