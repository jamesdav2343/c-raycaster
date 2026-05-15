#include "systems/transform.h"

void TransformSystemsImport(ecs_world_t* world)
{
    ECS_IMPORT(world, TransformComponents);

    ECS_MODULE(world, TransformSystems);
}