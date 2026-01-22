#include <SDL3/SDL.h>
#include <stdio.h>
#include "map.h"
#include "window.h"
#include "game_manager.h"

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
    SDL_Texture *texture;
    SDL_Event event;
    GameStatus *game_status;

    init("raycasting engine", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer, &game_status);

    MapGrid map_grid = {mapWidth, mapHeight};
    WindowDimensions window_dimensions = {SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_FRect test_rect = {0, 0, window_dimensions.width_pixels / map_grid.x_tiles_count, window_dimensions.height_pixels / map_grid.y_tiles_count};

    while (game_status->is_running)
    {
        handle_events(&event, game_status);
        update();
        render();
        draw_map(renderer, &test_rect, &map_grid);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    free(game_status);
    SDL_Quit();

    printf("Game closed");

    return 0;
}