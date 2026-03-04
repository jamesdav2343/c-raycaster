#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <stdio.h>
#include <stdbool.h>
#include <flecs.h>
#include <SDL3/SDL.h>

#define VECTOR2_ZERO (Vector2){0.0f, 0.0f}
#define VECTOR2_ONE (Vector2){1.0f, 1.0f}
#define VECTOR2_LEFT (Vector2){-1.0f, 0.0f}
#define VECTOR2_RIGHT (Vector2){1.0f, 0.0f}
#define VECTOR2_UP (Vector2){0.0f, -1.0f}
#define VECTOR2_DOWN (Vector2){0.0f, 1.0f}

#define TRANSFORM_ZERO (Transform){VECTOR2_ZERO, VECTOR2_ZERO, 0.0f}

ECS_COMPONENT_DECLARE(Transform);

typedef struct Vector2
{
    float x;
    float y;
} Vector2;

typedef struct Transform
{
    Vector2 position;
    Vector2 deltaPosition;
    float rotation;
} Transform;

typedef struct Sprite
{
    SDL_FRect rect;
} Sprite;

Vector2 vector2_add(Vector2 v1, Vector2 v2);

Vector2 vector2_subtract(Vector2 v1, Vector2 v2);

bool vector2_equals(Vector2 v1, Vector2 v2);

void vector2_print(Vector2 vector);

#endif