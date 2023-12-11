#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"
#include "imgui.h"
#include <SDL.h>
#include <cmath>
#include <math.h>
#include <stdio.h>
#include <vector>
#include <complex>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

void newton_fractal(ImDrawList* drawlist, int width, int height, int max_iter) {
    const double epsilon = 1e-6;

    const double x_min = -2.0;
    const double x_max = 2.0;
    const double y_min = -2.0;
    const double y_max = 2.0;

    const double x_step = (x_max - x_min) / (width - 1);
    const double y_step = (y_max - y_min) / (height - 1);

    for (int j = 0; j < height; ++j) {
        double y = y_min + y_step * j;

        for (int i = 0; i < width; ++i) {
            double x = x_min + x_step * i;

            std::complex<double> z(x, y);

            int iter = 0;
            double norm_z_cubed_minus_one = std::norm(z * z * z - 1.0);

            const ImVec2 point(i, height - j);
            while (iter < max_iter && norm_z_cubed_minus_one > epsilon) {
                z -= (z * z * z - 1.0) / (3.0 * z * z);
                ++iter;
                norm_z_cubed_minus_one = std::norm(z * z * z - 1.0);
            }

            if (iter == max_iter) {
                drawlist->AddRectFilled(point, ImVec2(i + 1, height - j + 1), IM_COL32(0, 0, 0, 255));
            } else {
                float intensity = static_cast<float>(iter) / static_cast<float>(max_iter);
                ImVec4 color(intensity, 0.5f, 1.0f - intensity, 1.0f);
                drawlist->AddRectFilled(point, ImVec2(i + 1, height - j + 1), ImGui::ColorConvertFloat4ToU32(color));
            }
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
        SDL_WINDOWPOS_CENTERED, 800, 600, window_flags);
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

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    bool done = false;
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
                static int counter = 0;

                ImGuiViewport *viewport = ImGui::GetMainViewport();
                auto size = viewport->WorkSize;
                ImGui::SetNextWindowPos(viewport->WorkPos);
                ImGui::SetNextWindowSize(size);

                ImGui::Begin(
                    "Hello, world!", NULL,
                    ImGuiWindowFlags_NoDecoration);

                const ImVec2 center{size.x / 2, size.y / 2};

                int max_iter = 15;
                newton_fractal(ImGui::GetWindowDrawList(), size.x, size.y, max_iter);

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