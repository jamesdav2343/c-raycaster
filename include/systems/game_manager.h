#ifndef GAME_MANAGER_SYSTEMS_H
#define GAME_MANAGER_SYSTEMS_H

#include "components/game_manager.h"
#include "components/input.h"
#include "map.h"
#include "player.h"
#include <SDL3/SDL.h>
#include <cjson/cJSON.h>
#include <flecs.h>

#define TITLE "raycaster"
#define CONFIG_FILE "config.json"

void handle_events(SDL_Event* event, GameStatus* game_status);

void game_manager_cleanup(ecs_world_t* world, void* ctx);

void GameManagerSystemsImport(ecs_world_t* world);

#endif