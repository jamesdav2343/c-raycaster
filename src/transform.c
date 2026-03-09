#include "transform.h"

ECS_COMPONENT_DECLARE(Position);
ECS_COMPONENT_DECLARE(Direction);
ECS_COMPONENT_DECLARE(Rotation);

typedef struct Rotation
{
    float angle;
} Rotation;

void TransformModuleImport(ecs_world_t *world)
{
    ECS_MODULE(world, TransformModule);

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Direction);
    ECS_COMPONENT_DEFINE(world, Rotation);
}

Vector2 vector2_add(Vector2 v1, Vector2 v2)
{
    return (Vector2){v1.x + v2.x, v1.y + v2.y};
}

Vector2 vector2_subtract(Vector2 v1, Vector2 v2)
{
    return (Vector2){v1.x - v2.x, v1.y - v2.y};
}

bool vector2_equals(Vector2 v1, Vector2 v2)
{
    return v1.x == v2.x && v1.y == v2.y;
}

void vector2_print(Vector2 vector)
{
    printf("(%f, %f)\n", vector.x, vector.y);
}

static float wrap_angle(float angle)
{
    angle = fmodf(angle, 2 * M_PI);
    if (angle < 0)
        angle += 2 * M_PI;

    return angle;
}

Rotation *create_rotation(float initial_value)
{
    Rotation *rotation = malloc(sizeof(Rotation));
    rotation->angle = initial_value;
    return rotation;
}

void rotation_set_angle(Rotation *rotation, float value)
{
    rotation->angle = wrap_angle(value);
}

float rotation_add_angle(Rotation *rotation, float delta)
{
    rotation->angle = wrap_angle(rotation->angle + delta);
    return rotation->angle;
}

float rotation_get_angle(Rotation *rotation)
{
    return rotation->angle;
}