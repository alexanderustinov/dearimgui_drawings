#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include <stdio.h>
#include <cmath>
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif
#include <cmath>

struct Vector {
    float x;
    float y;

    Vector() {
        this->x = 0;
        this->y = 0;
    }
    
    Vector(float x, float y) {
        this->x = x;
        this->y = y;
    }
        
    Vector sum(Vector v2) {
        this->x = this->x + v2.x;
        this->y = this->y + v2.y;
        return *this;
    }

    Vector turn(float fi) {
        float old_x = this->x;
        float old_y = this->y;
        this->x = old_x * cosf(fi * M_PI / 180.0f) - old_y * sinf(fi * M_PI / 180.0f);
        this->y = old_x * sinf(fi * M_PI / 180.0f) + old_y * cosf(fi * M_PI / 180.0f);
        return *this;
    }
        
    ImVec2 toImVec2() {
        ImVec2 G{x, y};
        return G;
    }
};