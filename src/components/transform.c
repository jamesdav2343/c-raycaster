#include "components/transform.h"

ECS_COMPONENT_DECLARE(Position);
ECS_COMPONENT_DECLARE(Direction);

void TransformComponentsImport(ecs_world_t* world)
{
    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Direction);

    ECS_MODULE(world, TransformComponents);
}