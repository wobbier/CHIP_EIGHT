#define SDL_MAIN_HANDLED
#include <SDL.h>

#include "Platform\Window.h"

int main(int argc, char** argv)
{
    Window window;

    while (!window.ShouldClose)
    {
        window.ProcessEvents();
    }

    return 0;
}