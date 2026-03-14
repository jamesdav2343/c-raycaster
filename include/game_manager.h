#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <SDL3/SDL.h>
#include <flecs.h>
#include "window.h"
#include "map.h"
#include "player.h"
#include "controller.h"
#include "raycaster.h"

#define GAME_TITLE "raycaster-engine"

extern ECS_COMPONENT_DECLARE(Renderer);
extern ECS_COMPONENT_DECLARE(Window);

typedef struct Renderer
{
    SDL_Renderer *ptr;
} Renderer;

typedef struct Window
{
    SDL_Window *ptr;
} Window;

void GameManagerModuleImport(ecs_world_t *world);

typedef struct GameStatus
{
    bool is_running;
} GameStatus;

void handle_events(SDL_Event *event, GameStatus *game_status);

#endif