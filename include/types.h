#ifndef TYPES_H
#define TYPES_H

#include "ht.h"
#include <stdlib.h>

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

typedef struct TextureData {
    const char* path;
    Vector2I size;
} TextureData;

typedef struct {
    double distance;
    int order;
} SpriteSortPair;

#endif
