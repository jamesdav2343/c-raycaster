#include <SDL3/SDL.h>
#include <stdio.h>
#include <flecs.h>
#include "map.h"
#include "window.h"
#include "game_manager.h"
#include "player.h"

#define FPS 60

int main(int argc, char *argv[])
{
    SDL_Window *window;
    SDL_Event event;
    GameStatus game_status;

    ecs_world_t *world;

    init("raycasting-engine", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer, &game_status, &world);

    ECS_IMPORT(world, PlayerModule);
    ECS_IMPORT(world, RaycasterModule);

    // Instantiates a player from a prefab
    ecs_entity_t player = ecs_new_w_pair(world, EcsIsA, Player);
    ecs_set(world, player, Position, {14, 160});
    ecs_set_name(world, player, PLAYER_ENTITY_NAME);

    // Instantiates the raycaster entity
    ecs_entity_t raycaster = ecs_new(world);
    ecs_add(world, raycaster, Renderer);

    ecs_set_target_fps(world, FPS);

    while (game_status.is_running)
    {
        handle_events(&event, &game_status);

        ecs_progress(world, 0);
    }

    ecs_delete(world, raycaster);
    ecs_fini(world);

    SDL_DestroyWindow(window);

    return 0;
}