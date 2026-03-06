#include <SDL3/SDL.h>
#include <stdio.h>
#include "map.h"
#include "window.h"
#include "game_manager.h"
#include "player.h"
#include "frame_rate.h"
#include <flecs.h>

int main(int argc, char *argv[])
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Event event;
    GameStatus game_status;

    ecs_world_t *world;

    init("raycasting-engine", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer, &game_status, &world);

    ECS_IMPORT(world, TransformModule);
    ECS_IMPORT(world, PlayerModule);

    // Instantiates a player from a prefab
    ecs_entity_t p = ecs_new_w_pair(world, EcsIsA, Player);

    SDL_FRect tile = {0, 0, TILE_PIXEL_COUNT, TILE_PIXEL_COUNT};
    MapData map_data = {COLS, ROWS, &tile};
    memccpy(&map_data.grid, &map, ROWS, sizeof(map_data.grid));

    FrameData frame_data = (FrameData){0, 0, 0, 0, 0, 0};

    while (game_status.is_running)
    {
        frame_data.frame_start_time = SDL_GetTicks();

        handle_events(&event, &game_status);
        // render(renderer, world, p, &map_data);

        ecs_progress(world, 0);

        update_frame_data(&frame_data);
        limit_frame_rate(frame_data.delta);
    }

    ecs_fini(world);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    printf("Game closed.\n");

    return 0;
}