#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <stdio.h>
#include <stdbool.h>
#include <flecs.h>
#include <SDL3/SDL.h>
#include <math.h>

#define VECTOR2_ZERO (Vector2){0.0f, 0.0f}
#define VECTOR2_ONE (Vector2){1.0f, 1.0f}
#define VECTOR2_LEFT (Vector2){-1.0f, 0.0f}
#define VECTOR2_RIGHT (Vector2){1.0f, 0.0f}
#define VECTOR2_UP (Vector2){0.0f, -1.0f}
#define VECTOR2_DOWN (Vector2){0.0f, 1.0f}

extern ECS_COMPONENT_DECLARE(Position);
extern ECS_COMPONENT_DECLARE(Direction);
extern ECS_COMPONENT_DECLARE(Rotation);

void TransformModuleImport(ecs_world_t *world);

typedef struct Vector2
{
    float x;
    float y;
} Vector2;

// Represents a position in a 2D plane. Holds an x and y coordinate.
typedef Vector2 Position;

// Represents the orientation of a position vector.
typedef Vector2 Direction;

Vector2 vector2_add(Vector2 v1, Vector2 v2);

Vector2 vector2_subtract(Vector2 v1, Vector2 v2);

bool vector2_equals(Vector2 v1, Vector2 v2);

void vector2_print(Vector2 vector);

typedef struct Rotation Rotation;

Rotation *create_rotation(float initial_angle);

void rotation_set_angle(Rotation *rotation, float value);

float rotation_add_angle(Rotation *rotation, float delta);

float rotation_get_angle(Rotation *rotation);

#endif