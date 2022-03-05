#pragma once
#define SDL_MAIN_HANDLED
#include <SDL.h>

struct Window
{
    Window(const char* inName, int inWidth, int inHeight);

    void ProcessEvents();

    SDL_Window* WindowPtr = nullptr;
    SDL_Renderer* Renderer = nullptr;
    bool ShouldClose = false;

    int Width  = 800;
    int Height = 600;
};