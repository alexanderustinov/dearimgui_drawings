#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"
#include "imgui.h"
#include <SDL.h>
#include <cmath>
#include <math.h>
#include <stdio.h>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

void dragon(float x1, float y1, float x2, float y2, int depth, ImDrawList* drawlist) {
    if (depth == 0) {
        drawlist->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), IM_COL32(255, 255, 255, 255), 1.0f);
    } else {
        float tx = (x1 + x2) / 2 + (y2 - y1) / 2;
        float ty = (y1 + y2) / 2 - (x2 - x1) / 2;
        dragon(x2, y2, tx, ty, depth - 1, drawlist);
        dragon(x1, y1, tx, ty, depth - 1, drawlist);
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

  // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
  SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

  // Create window with graphics context
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
  SDL_GL_SetSwapInterval(0); // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // Setup Platform/Renderer backends
  ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Our state
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // Main loop
  bool done = false;
  static float angle = 0;
  const int length = 100;
  while (!done) {
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to
    // tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to
    // your main application, or clear/overwrite your copy of the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input
    // data to your main application, or clear/overwrite your copy of the
    // keyboard data. Generally you may always pass all inputs to dear imgui,
    // and hide them from your application based on those two flags.
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

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    {
        
      static float speed = 0.5f;
      static int counter = 0;

      // resize future window to fit viewport
      ImGuiViewport *viewport = ImGui::GetMainViewport();
      auto size = viewport->WorkSize;
      ImGui::SetNextWindowPos(viewport->WorkPos);
      ImGui::SetNextWindowSize(size);

      ImGui::Begin(
          "Hello, world!", NULL,
          ImGuiWindowFlags_NoDecoration); // Create a window called "Hello,
                                          // world!" and append into it.

      const ImVec2 center{size.x / 2, size.y / 2};
      const ImVec2 other{
          center.x + static_cast<float>(std::cos(angle) * length),
          center.y + static_cast<float>(std::sin(angle) * length)};
      angle = angle + speed * 10 * M_PI / 180;

      auto drawlist = ImGui::GetWindowDrawList();
      dragon(center.x, center.y, other.x, other.y, 14, drawlist);
      drawlist->AddLine(center, other, IM_COL32(255, 0, 0, 255), 5.0f);
      drawlist->AddCircleFilled(center, 15, IM_COL32(255, 255, 255, 128));

      //   ImGui::Text("This is some useful text."); // Display some text
      //   (you can
      //                                             // use a format strings
      //                                             too)
      //   ImGui::Checkbox("Another Window", &show_another_window);

      ImGui::SliderFloat("Speed", &speed, 0.0f,
                         1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
                                //   ImGui::ColorEdit3(
                                //       "clear color",
      //       (float *)&clear_color); // Edit 3 floats representing a color

      //   if (ImGui::Button("Button")) // Buttons return true when clicked
      //   (most
      //                                // widgets return true when
      //                                edited/activated)
      counter++;
      //   ImGui::SameLine();
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

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
