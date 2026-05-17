#include "game_manager.h"
#include "general_utils.h"

ECS_COMPONENT_DECLARE(Window);
ECS_COMPONENT_DECLARE(Renderer);

void GameManagerModuleImport(ecs_world_t* world)
{
    cJSON* config = load_config_json(CONFIG_FILE);

    printf("%s\n", cJSON_Print(config));

    ECS_MODULE(world, GameManager);

    ECS_COMPONENT_DEFINE(world, Window);
    ECS_COMPONENT_DEFINE(world, Renderer);

    SDL_Window* window;
    SDL_Renderer* renderer;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return;
    }

    window = SDL_CreateWindow(TITLE, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);

    if (!window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window: %s", SDL_GetError());
        return;
    }

    SDL_Surface* window_surface = SDL_GetWindowSurface(window);
    renderer = SDL_CreateSoftwareRenderer(window_surface);

    if (!renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create software renderer: %s", SDL_GetError());
        return;
    }

    ecs_add_id(world, ecs_id(Window), EcsSingleton);
    ecs_singleton_set(world, Window, { window });

    ecs_add_id(world, ecs_id(Renderer), EcsSingleton);
    ecs_singleton_set(world, Renderer, { renderer });

    ecs_atfini(world, game_manager_cleanup, NULL);
}

void handle_events(SDL_Event* event, GameStatus* game_status)
{
    SDL_PollEvent(event);

    if (event->type == SDL_EVENT_QUIT) {
        game_status->is_running = false;
    }
}

void game_manager_cleanup(ecs_world_t* world, void* ctx)
{
    SDL_Renderer* renderer = ecs_singleton_get(world, Renderer)->value;
    SDL_Window* window = ecs_singleton_get(world, Window)->value;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    printf("Game closed.\n");
}