#include <SDL3/SDL.h>
#include <stdio.h>
#include "map.h"
#include "window.h"
#include "game_manager.h"
#include "player.h"

Uint8 map[rows][cols] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 2, 2, 2, 0, 0, 0, 0, 1},
    {1, 0, 2, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 2, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 2, 2, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 2, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 2, 0, 0, 1},
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
    GameStatus game_status;

    char buffer[50];

    init("raycasting-engine", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer, &game_status);

    WindowDimensions window_dimensions = {SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_FRect test_rect = {0, 0, window_dimensions.height_pixels / rows, window_dimensions.height_pixels / rows};

    MapData map_data = {cols, rows, &test_rect};
    memccpy(&map_data.grid, &map, rows, sizeof(map_data.grid));

    PlayerData player_data = {&(Vector2){0, 0}};

    Uint64 frame_start, delta;
    int fps = 0;
    Uint64 last_time = 0;
    Uint64 ms_per_frame = 1000.0 / FPS;

    while (game_status.is_running)
    {
        fps++;
        frame_start = SDL_GetTicks();

        handle_events(&event, &game_status);
        update(&player_data);
        render(renderer, &map_data, &player_data);

        delta = SDL_GetTicks() - frame_start;

        if (frame_start > last_time + 1000)
        {
            last_time = frame_start;
            sprintf(buffer, "fps: %d\n", fps);
            SDL_SetWindowTitle(window, buffer);
            fps = 0;
        }

        if (ms_per_frame > delta)
        {
            SDL_Delay(ms_per_frame - delta);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    printf("Game closed.\n");

    return 0;
}