#ifndef GAME_MANAGER_COMPONENTS_H
#define GAME_MANAGER_COMPONENTS_H

#include "data_structures/ht.h"
#include "types.h"
#include <SDL3/SDL.h>
#include <flecs.h>

extern ECS_COMPONENT_DECLARE(Renderer);
extern ECS_COMPONENT_DECLARE(Window);
extern ECS_COMPONENT_DECLARE(VideoConfig);
extern ECS_COMPONENT_DECLARE(TexturesConfig);
extern ECS_TAG_DECLARE(Config);

typedef struct Renderer {
    SDL_Renderer* value;
} Renderer;

typedef struct Window {
    SDL_Window* value;
} Window;

typedef struct GameStatus {
    bool is_running;
} GameStatus;

void GameManagerComponentsImport(ecs_world_t* world);

#endif