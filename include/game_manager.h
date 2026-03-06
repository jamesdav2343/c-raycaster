#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <SDL3/SDL.h>
#include "map.h"
#include "player.h"
#include "controller.h"
#include "raycaster.h"
#include "flecs.h"

// Holds data about the overall game state
typedef struct GameStatus
{
    bool is_running;
} GameStatus;

int init(const char *title, int window_width, int window_height, SDL_WindowFlags window_flags, SDL_Window **window, SDL_Renderer **renderer, GameStatus *game_status, ecs_world_t **world);
void render(SDL_Renderer *renderer, ecs_world_t *world, ecs_entity_t player, MapData *map_data);
void handle_events(SDL_Event *event, GameStatus *game_status);

#endif