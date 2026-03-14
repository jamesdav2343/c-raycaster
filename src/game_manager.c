#include "game_manager.h"

ECS_COMPONENT_DECLARE(Window);
ECS_COMPONENT_DECLARE(Renderer);

void GameManagerModuleImport(ecs_world_t *world)
{
    ECS_MODULE(world, GameManager);

    ECS_COMPONENT_DEFINE(world, Window);
    ECS_COMPONENT_DEFINE(world, Renderer);

    SDL_Window *window;
    SDL_Renderer *renderer;

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return;
    }

    if (!SDL_CreateWindowAndRenderer(GAME_TITLE, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
        return;
    }

    ecs_add_id(world, ecs_id(Window), EcsSingleton);
    ecs_singleton_set(world, Window, {window});

    ecs_add_id(world, ecs_id(Renderer), EcsSingleton);
    ecs_singleton_set(world, Renderer, {renderer});
}

void handle_events(SDL_Event *event, GameStatus *game_status)
{
    SDL_PollEvent(event);

    if (event->type == SDL_EVENT_QUIT)
    {
        game_status->is_running = false;
    }

    Vector2 inputDirection = get_input_direction();
}