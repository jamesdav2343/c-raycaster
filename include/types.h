#ifndef TYPES_H
#define TYPES_H

// Enum to represent a wall's orientation.
enum Orientation { VERTICAL = 0, HORIZONTAL = 1 };

// A 2D vector using floating-point coordinates.
typedef struct Vector2 {
    float x;
    float y;
} Vector2;

// A 2D vector using integer coordinates.
typedef struct Vector2I {
    int x;
    int y;
} Vector2I;

#endif
