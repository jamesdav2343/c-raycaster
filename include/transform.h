#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <stdio.h>
#include <stdbool.h>
#include <flecs.h>
#include <SDL3/SDL.h>

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

extern const Vector2 VECTOR2_ZERO;
extern const Vector2 VECTOR2_ONE;
extern const Vector2 VECTOR2_LEFT;
extern const Vector2 VECTOR2_RIGHT;
extern const Vector2 VECTOR2_UP;
extern const Vector2 VECTOR2_DOWN;

extern const Transform DEFAULT_TRANSFORM;

Vector2 vector2_add(Vector2 v1, Vector2 v2);

Vector2 vector2_subtract(Vector2 v1, Vector2 v2);

bool vector2_equals(Vector2 v1, Vector2 v2);

void vector2_print(Vector2 *vector);

#endif