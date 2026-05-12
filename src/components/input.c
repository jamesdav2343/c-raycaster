#include "components/input.h"

ECS_COMPONENT_DECLARE(Controller);

void ControllerComponentsImport(ecs_world_t* world)
{
    printf("\ncontroller components import here\n");
    // Calling this after ECS_MODULE breaks stuff for some reason
    ECS_COMPONENT_DEFINE(world, Controller);
    ECS_MODULE(world, ControllerComponents);
}