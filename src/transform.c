#include "transform.h"

ECS_COMPONENT_DECLARE(Position);
ECS_COMPONENT_DECLARE(DeltaPosition);
ECS_COMPONENT_DECLARE(Rotation);

void TransformModuleImport(ecs_world_t *world)
{
    ECS_MODULE(world, TransformModule);

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, DeltaPosition);
    ECS_COMPONENT_DEFINE(world, Rotation);
}

Position vector2_add(Position v1, Position v2)
{
    return (Position){v1.x + v2.x, v1.y + v2.y};
}

Position vector2_subtract(Position v1, Position v2)
{
    return (Position){v1.x - v2.x, v1.y - v2.y};
}

bool vector2_equals(Position v1, Position v2)
{
    return v1.x == v2.x && v1.y == v2.y;
}

void vector2_print(Position vector)
{
    printf("(%f, %f)\n", vector.x, vector.y);
}