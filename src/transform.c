#include "transform.h"

Vector2 vector2_add(Vector2 v1, Vector2 v2)
{
    return (Vector2){v1.x + v2.x, v1.y + v2.y};
}

void vector2_print(Vector2 *vector)
{
    printf("(%f, %f)\n", vector->x, vector->y);
}