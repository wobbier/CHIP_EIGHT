#include "Window.h"

Window::Window(const char* inName, int inWidth, int inHeight)
    : Width(inWidth)
    , Height(inHeight)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    WindowPtr = SDL_CreateWindow(inName, 200, 200, Width, Height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    Renderer = SDL_CreateRenderer(WindowPtr, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
}

void Window::ProcessEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            ShouldClose = true;
        }
    }
}
