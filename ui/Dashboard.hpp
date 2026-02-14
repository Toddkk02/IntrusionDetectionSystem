#pragma once
#include <SDL2/SDL.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "../core/AlertQueue.hpp"

class Dashboard {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    AlertQueue& queue;
    std::deque<Alert> alertLog;
    bool running;
    time_t startTime;
public:
    Dashboard(AlertQueue& queue);
    ~Dashboard();
    void init();
    void render();
    void run();
    void exportCSV();
};