#include "components/input.h"

ECS_COMPONENT_DECLARE(Controller);

void InputComponentsImport(ecs_world_t* world)
{
    // Calling this after ECS_MODULE breaks stuff for some reason
    ECS_COMPONENT_DEFINE(world, Controller);

    ECS_MODULE(world, InputComponents);
}