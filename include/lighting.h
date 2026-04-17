#ifndef LIGHTING_H
#define LIGHTING_H

#include "map.h"
#include "transform.h"

extern float light_map[ROWS][COLS];

void bake_light_map();

#endif