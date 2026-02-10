#include <SDL3/SDL.h>
#include <stdio.h>
#include "map.h"
#include "window.h"
#include "game_manager.h"
#include "player.h"
#include "frame_rate.h"

int main(int argc, char *argv[])
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Event event;
    GameStatus game_status;

    init("raycasting-engine", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer, &game_status);

    SDL_FRect tile = {0, 0, TILE_PIXEL_COUNT, TILE_PIXEL_COUNT};

    MapData map_data = {COLS, ROWS, &tile};
    memccpy(&map_data.grid, &map, ROWS, sizeof(map_data.grid));

    PlayerData player_data = {
        (Vector2){0, 0},
        (Vector2){0, 0},
        0,
        (SDL_FRect){0, 0, 10, 10},
    };

    FrameData frame_data = (FrameData){0, 0, 0, 0, 0, 0};

    while (game_status.is_running)
    {
        frame_data.frame_start_time = SDL_GetTicks();

        handle_events(&event, &game_status);
        update(&player_data);
        render(renderer, &map_data, &player_data);

        update_frame_data(&frame_data);
        limit_frame_rate(frame_data.delta);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    printf("Game closed.\n");

    return 0;
}