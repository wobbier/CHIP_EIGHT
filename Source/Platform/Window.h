#pragma once
#define SDL_MAIN_HANDLED
#include <SDL.h>

struct Window
{
    Window();

    void ProcessEvents();

    SDL_Window* WindowPtr = nullptr;
    bool ShouldClose = false;
};