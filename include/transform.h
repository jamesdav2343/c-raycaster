#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <stdio.h>

typedef struct Vector2
{
    float x;
    float y;
} Vector2;

Vector2 vector2_add(Vector2 v1, Vector2 v2);

void vector2_print(Vector2 *vector);

#endif