#include "transform.h"

const Vector2 VECTOR2_ZERO = {0, 0};
const Vector2 VECTOR2_ONE = {1, 1};
const Vector2 VECTOR2_LEFT = {-1, 0};
const Vector2 VECTOR2_RIGHT = {1, 0};
const Vector2 VECTOR2_UP = {0, -1};
const Vector2 VECTOR2_DOWN = {0, 1};

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

void vector2_print(Vector2 *vector)
{
    printf("(%f, %f)\n", vector->x, vector->y);
}