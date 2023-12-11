#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include <stdio.h>
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

// This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

#include "Ве_крест_крест.h"


бестолочь вичек_рисовать(Холст *холстик, целина глубина, Пара &координаты, малый_дробь размер, розсуд вертикально, Четвёрка &цвет) {
    коли (глубина == 0) {
        холстик->Прямоугольник(координаты, Пара(координаты.x + размер, координаты.y + размер), Четвёрка_в_цвет(цвет));
    } отнюдь {
        малый_дробь треть = размер / 3;
        для (целина i=0; i<3;i++) {
            для (целина j=0; j<3;j++) {
                розсуд нужно_вертикально = (i == 1 || j == 1) && вертикально;
                розсуд нужно_горизонтально = ((i + j) % 2 == 0) && !вертикально;
                коли (!(нужно_вертикально || нужно_горизонтально)) {
                    добить_ящеров;
                }
                Пара новые_координаты = Пара(координаты.x + i * треть, координаты.y + j * треть);
                вичек_рисовать(холстик, глубина-1, новые_координаты, треть, вертикально, цвет);
            }
        }
    }
}

// Main code
царь_батюшка_главный(целина, char**)
{
    // Setup SDL
    коли (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        воздать -1;
    }

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
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
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL2+OpenGL3 example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    Диковинка::CreateContext();
    ImGuiIO& io = Диковинка::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear imGui style
    Диковинка::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    целина глубина = 7;
    розсуд вертикально = true;
    Пара начальные_координаты = Пара(100, 100);
    Четвёрка цвет_фрактала = Четвёрка(1.0f, 1.0f, 1.0f, 1.0f);
    Четвёрка цвет_фона = Четвёрка(0.0f, 0.0f, 0.0f, 1.00f);

    // Main loop
    розсуд всё = кривда;
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    покуда (!всё)
#endif
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;

        покуда (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            коли (event.type == SDL_QUIT)
                всё = правда;
            коли (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                всё = правда;
        }

        // Start the Dear imgui frame
        И_Раз();
        И_Два();
        Диковинка::Кадр();

        Ставни *ставеньки = Диковинка::ДайСтавни();

        {
            Диковинка::Начнём("фрактал Вичека", туда_не_знаю_куда, НеВыпендриваться + Стоять + Голый);
            Диковинка::ДатьПоложение(ставеньки->Положение);
            Диковинка::ДатьРазмер(ставеньки->Размер);

            Холст *холстик = Диковинка::ДайХолст();

            вичек_рисовать(холстик, глубина, начальные_координаты, 600, вертикально, цвет_фрактала);
            Диковинка::СказочкеКонец();
        }

        {
            Диковинка::Начнём("FPS (7 levels)");                          // Create a window called "Hello, world!" and append into it.

            Диковинка::Письмо("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            Диковинка::СказочкеКонец();
        }

        // Rendering
        Диковинка::Показывай();
        glViewport(0, 0, (целина)io.DisplaySize.x, (целина)io.DisplaySize.y);
        glClearColor(цвет_фона.x * цвет_фона.w, цвет_фона.y * цвет_фона.w, цвет_фона.z * цвет_фона.w, цвет_фона.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(Диковинка::GetDrawData());
        SDL_GL_SwapWindow(window);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    Диковинка::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    воздать 0;
}
