#include "ecs.h"

void rce_init_ecs(ecs_world_t *world)
{
    ECS_COMPONENT_DEFINE(world, Transform);
}