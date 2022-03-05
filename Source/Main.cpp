#define SDL_MAIN_HANDLED
#include <SDL.h>

#include "Platform\Window.h"
#include "Core\ChipEight.h"

#define C8_WIDTH 64
#define C8_HEIGHT 32

int main(int argc, char** argv)
{
    Window window("ChipEight", 800, 400);
    ChipEight Program;
    Program.Initialize();

    // Call this for different resolutions
    SDL_RenderSetLogicalSize(window.Renderer, C8_WIDTH, C8_HEIGHT);

    while (!window.ShouldClose)
    {
        window.ProcessEvents();
        Program.EmulateCycle();

        SDL_SetRenderDrawColor(window.Renderer, 0, 0, 0, 0);

        SDL_RenderClear(window.Renderer);


        SDL_SetRenderDrawColor(window.Renderer, 255, 255, 255, 255);
        for (int i = 0; i < C8_HEIGHT; ++i)
        {
            for (int j = 0; j < C8_WIDTH; ++j)
            {
                if (Program.GfxBuffer[(i * 64) + j] == 1)
                {
                    SDL_RenderDrawPoint(window.Renderer, j, i);
                }
            }
        }

        SDL_RenderPresent(window.Renderer);
    }

    return 0;
}