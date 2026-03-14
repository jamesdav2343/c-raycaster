#include "controller.h"

ECS_COMPONENT_DECLARE(Controller);
ECS_SYSTEM_DECLARE(ControllerUpdate);

void ControllerModuleImport(ecs_world_t *world)
{
    ECS_MODULE(world, ControllerModule);

    ECS_COMPONENT_DEFINE(world, Controller);
    ECS_SYSTEM_DEFINE(world, ControllerUpdate, EcsOnUpdate, Controller);
}

void ControllerUpdate(ecs_iter_t *it)
{
    Controller *controller = ecs_field(it, Controller, 0);

    for (int i = 0; i < it->count; i++)
    {
        Vector2 input_direction = get_input_direction();

        controller[i].x = input_direction.x;
        controller[i].y = input_direction.y;
    }
}

// Returns a vector of the current keyboard input direction.
Vector2 get_input_direction()
{
    const bool *key_states = SDL_GetKeyboardState(NULL);

    int x_dir = 0;
    int y_dir = 0;

    if (key_states[SDL_SCANCODE_W])
        y_dir--;
    if (key_states[SDL_SCANCODE_A])
        x_dir--;
    if (key_states[SDL_SCANCODE_S])
        y_dir++;
    if (key_states[SDL_SCANCODE_D])
        x_dir++;

    return (Vector2){x_dir, y_dir};
}