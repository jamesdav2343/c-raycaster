#ifndef GENERAL_UTILS_H
#define GENERAL_UTILS_H

#include <SDL3/SDL.h>

#define ALPHA_OPAQUE_HEX 0xFF000000

/**
 * interpolate 2 RGB colors
 * @param color1    integer containing color as 0x00RRGGBB
 * @param color2    integer containing color as 0x00RRGGBB
 * @param fraction  how much interpolation (0..1)
 * - 0: full color 1
 * - 1: full color 2
 * @return the new color after interpolation
 *
 * Credit to @Carlos Barcellos on Stack Overflow
 * https://stackoverflow.com/a/21010385
 */
Uint32 interpolate(int color1, int color2, float fraction);

#endif