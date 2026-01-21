#include <SDL3/SDL.h>
#include <stdio.h>
#include "map.h"
#include "window.h"

#define mapWidth 10
#define mapHeight 10
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

Uint8 map[mapHeight][mapWidth] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 2, 2, 2, 0, 0, 1},
    {1, 0, 0, 0, 2, 0, 2, 0, 0, 1},
    {1, 0, 0, 0, 2, 2, 2, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

int main(int argc, char *argv[])
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *surface;
    SDL_Event event;
    SDL_Texture *texture;
    bool isRunning = true;

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return -1;
    }

    if (!SDL_CreateWindowAndRenderer("Raycasting engine", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
        return -1;
    }

    MapGrid mapData = {mapWidth, mapHeight};
    WindowDimensions windowDimensions = {SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_FRect testRect = {0, 0, windowDimensions.widthPixels / mapData.xTilesCount, windowDimensions.heightPixels / mapData.yTilesCount};

    while (isRunning)
    {
        SDL_PollEvent(&event);

        if (event.type == SDL_EVENT_QUIT)
        {
            isRunning = false;
        }
        else if (event.type == SDL_EVENT_WINDOW_RESIZED)
        {
            update_window(window, &windowDimensions);

            testRect.w = windowDimensions.widthPixels / mapData.xTilesCount;
            testRect.h = windowDimensions.heightPixels / mapData.yTilesCount;
        }

        draw_map(renderer, &testRect, &mapData);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}