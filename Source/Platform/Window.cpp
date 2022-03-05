#include "Window.h"

Window::Window()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    WindowPtr = SDL_CreateWindow("ChipEight", 200, 200, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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
