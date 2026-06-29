#ifndef TYPES_H
#define TYPES_H

#include "data_structures/ht.h"
#include <stdlib.h>

// Enum to represent a wall's orientation.
enum Orientation { NORTH = 0, SOUTH = 1, EAST = 2, WEST = 3 };

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

// -- Config components --

// Video config.
typedef struct VideoConfig {
    Vector2I screen_size;
    int fps_cap;
    bool enable_lighting;
} VideoConfig;

// Textures config.
typedef struct TexturesConfig {
    ht* config;
} TexturesConfig;

// Textures themselves
typedef struct Textures {
    ht* table;
} Textures;

typedef struct {
    double distance;
    int order;
} SpriteSortPair;

#endif
