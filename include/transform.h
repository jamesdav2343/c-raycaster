#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <stdio.h>
#include <stdbool.h>
#include <flecs.h>
#include <SDL3/SDL.h>
#include <math.h>

#define VECTOR2_ZERO (Position){0.0f, 0.0f}
#define VECTOR2_ONE (Position){1.0f, 1.0f}
#define VECTOR2_LEFT (Position){-1.0f, 0.0f}
#define VECTOR2_RIGHT (Position){1.0f, 0.0f}
#define VECTOR2_UP (Position){0.0f, -1.0f}
#define VECTOR2_DOWN (Position){0.0f, 1.0f}

extern ECS_COMPONENT_DECLARE(Position);
extern ECS_COMPONENT_DECLARE(Rotation);

void TransformModuleImport(ecs_world_t *world);

typedef struct Position
{
    float x;
    float y;
} Position;

Position vector2_add(Position v1, Position v2);

Position vector2_subtract(Position v1, Position v2);

bool vector2_equals(Position v1, Position v2);

void vector2_print(Position vector);

typedef struct Rotation Rotation;

Rotation *create_rotation(float initial_angle);

void rotation_set_angle(Rotation *rotation, float value);

float rotation_add_angle(Rotation *rotation, float delta);

float rotation_get_angle(Rotation *rotation);

#endif