#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <stdio.h>
#include <stdbool.h>
#include <flecs.h>
#include <SDL3/SDL.h>

#define VECTOR2_ZERO (Position){0.0f, 0.0f}
#define VECTOR2_ONE (Position){1.0f, 1.0f}
#define VECTOR2_LEFT (Position){-1.0f, 0.0f}
#define VECTOR2_RIGHT (Position){1.0f, 0.0f}
#define VECTOR2_UP (Position){0.0f, -1.0f}
#define VECTOR2_DOWN (Position){0.0f, 1.0f}

extern ECS_COMPONENT_DECLARE(Position);
extern ECS_COMPONENT_DECLARE(Rotation);

typedef struct Position
{
    float x;
    float y;
} Position;

typedef struct Rotation
{
    float rotation;
} Rotation;

void TransformModuleImport(ecs_world_t *world);

Position vector2_add(Position v1, Position v2);

Position vector2_subtract(Position v1, Position v2);

bool vector2_equals(Position v1, Position v2);

void vector2_print(Position vector);

#endif