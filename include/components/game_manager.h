#ifndef GAME_MANAGER_COMPONENTS_H
#define GAME_MANAGER_COMPONENTS_H

#include "ht.h"
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

// -- Config components --

// Video config.
typedef struct VideoConfig {
    Vector2I screen_size;
    int fps_cap;
} VideoConfig;

// Textures config.
typedef struct TexturesConfig {
    ht* table;
} TexturesConfig;

typedef struct TextureData {
    const char* path;
    Vector2I size;
} TextureData;

void GameManagerComponentsImport(ecs_world_t* world);

#endif