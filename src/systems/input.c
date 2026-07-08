#include "systems/input.h"

ECS_SYSTEM_DECLARE(ControllerUpdate);

void InputSystemsImport(ecs_world_t* world)
{
    ECS_IMPORT(world, InputComponents);

    ECS_SYSTEM_DEFINE(world, ControllerUpdate, EcsOnUpdate, Controller);

    ECS_MODULE(world, InputSystems);
}

// Returns a vector of the current keyboard input direction.
static Vector2I get_input_movement_direction()
{
    const bool* key_states = SDL_GetKeyboardState(NULL);

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

    return (Vector2I) { x_dir, y_dir };
}

static int get_input_viewport_direction()
{
    const bool* key_states = SDL_GetKeyboardState(NULL);
    int angle_dir = 0;

    if (key_states[SDL_SCANCODE_Q])
        angle_dir--;
    if (key_states[SDL_SCANCODE_E])
        angle_dir++;

    return angle_dir;
}

void ControllerUpdate(ecs_iter_t* it)
{
    Controller* controller = ecs_field(it, Controller, 0);

    for (int i = 0; i < it->count; i++) {
        Vector2I movement_dir = get_input_movement_direction();
        controller[i].movement_dir.x = movement_dir.x;
        controller[i].movement_dir.y = movement_dir.y;

        int viewport_dir = get_input_viewport_direction();
        controller[i].viewport_dir = get_input_viewport_direction();
    }
}