#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "components/input.h"
#include "map.h"
#include "player.h"
#include "window.h"
#include <SDL3/SDL.h>
#include <flecs.h>

#define TITLE "raycaster"
#define CONFIG_FILE "config.json"

extern ECS_COMPONENT_DECLARE(Renderer);
extern ECS_COMPONENT_DECLARE(Window);

typedef struct Renderer {
    SDL_Renderer* value;
} Renderer;

typedef struct Window {
    SDL_Window* value;
} Window;

void GameManagerModuleImport(ecs_world_t* world);

typedef struct GameStatus {
    bool is_running;
} GameStatus;

void handle_events(SDL_Event* event, GameStatus* game_status);

void game_manager_cleanup(ecs_world_t* world, void* ctx);

#endif