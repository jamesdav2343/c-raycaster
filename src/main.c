#include "data_structures/ht.h"
#include "lighting.h"
#include "map.h"
#include "player.h"
#include "systems/game_manager.h"
#include "systems/input.h"
#include "systems/raycaster.h"
#include "systems/sprite.h"
#include "systems/transform.h"
#include <SDL3/SDL.h>
#include <flecs.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    ecs_world_t* world = ecs_init();

    ECS_IMPORT(world, GameManagerSystems);
    ECS_IMPORT(world, TransformSystems);
    ECS_IMPORT(world, InputSystems);
    ECS_IMPORT(world, RaycasterSystems);
    ECS_IMPORT(world, SpriteSystems);

    bake_light_map();
    const VideoConfig* video_config = ecs_singleton_get(world, VideoConfig);

    SDL_Event event;
    GameStatus game_status;

    game_status.is_running = true;

    ECS_IMPORT(world, PlayerModule);

    // Instantiates a player from a prefab
    ecs_entity_t player = ecs_new_w_pair(world, EcsIsA, Player);
    ecs_set(world, player, Position, { 5, 5 });
    ecs_set_name(world, player, PLAYER_ENTITY_NAME);

    // Instantiates the raycaster entity
    ecs_entity_t raycaster = ecs_new(world);
    ecs_add(world, raycaster, Raycaster);

    ecs_set_target_fps(world, video_config->fps_cap);

    while (game_status.is_running) {
        handle_events(&event, &game_status);

        ecs_progress(world, 0);
    }

    ecs_delete(world, raycaster);
    ecs_fini(world);

    return 0;
}