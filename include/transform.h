#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <stdio.h>
#include <stdbool.h>

typedef struct Vector2
{
    float x;
    float y;
} Vector2;

extern const Vector2 VECTOR2_ZERO;
extern const Vector2 VECTOR2_ONE;
extern const Vector2 VECTOR2_LEFT;
extern const Vector2 VECTOR2_RIGHT;
extern const Vector2 VECTOR2_UP;
extern const Vector2 VECTOR2_DOWN;

Vector2 vector2_add(Vector2 v1, Vector2 v2);

Vector2 vector2_subtract(Vector2 v1, Vector2 v2);

bool vector2_equals(Vector2 v1, Vector2 v2);

void vector2_print(Vector2 *vector);

#endif