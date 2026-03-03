#include "game_manager.h"

int init(const char *title, int window_width, int window_height, SDL_WindowFlags window_flags, SDL_Window **window, SDL_Renderer **renderer, GameStatus *game_status, ecs_world_t **world)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return -1;
    }

    if (!SDL_CreateWindowAndRenderer(title, window_width, window_height, SDL_WINDOW_RESIZABLE, window, renderer))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
        return -1;
    }

    *world = ecs_init();
    rce_init_ecs(*world);

    game_status->is_running = true;

    return 0;
}

void update(ecs_world_t *world, PlayerData *player_data, ecs_entity_t player)
{
    update_player(world, player_data, player);
}

void render(SDL_Renderer *renderer, MapData *map_data, PlayerData *player_data)
{
    // draw_map(renderer, map_data);
    draw_player(renderer, player_data);
    draw_rays(renderer, player_data);

    SDL_RenderPresent(renderer);
    SDL_RenderClear(renderer);
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